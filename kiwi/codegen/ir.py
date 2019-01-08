from functools import partial
from functools import reduce

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace

from llvmlite import ir

debug_mode = True
trace = partial(trace, mode=debug_mode, name='[llvm_codegen] ')

n = None


@dataclass
class UndefinedBuiltin(Exception):
    builtin: Builtin


@dataclass
class ArgumentSizeMismatch(Exception):
    nargs: int
    args: List[Expression]


def ir_add_int(builder: ir.IRBuilder, args):
    return builder.add(args[0], args[1])


def ir_mult_int(builder: ir.IRBuilder, args):
    return builder.mul(args[0], args[1])


def ir_div_int(builder: ir.IRBuilder, args):
    return builder.sdiv(args[0], args[1])


def ir_sub_int(builder: ir.IRBuilder, args):
    return builder.sub(args[0], args[1])


def ir_return_fun(builder: ir.IRBuilder, args):
    return builder.ret(args[0])


def ir_make_int(builder: ir.IRBuilder, val):
    return ir.Constant(val)


def ir_make_variable():
    pass


def ir_make_lambda():
    pass


def ir_make_struct():
    pass


def ir_make_union():
    pass


builtins = {
    '+': (2, ir_add_int),
    '*': (2, ir_mult_int),
    '/': (2, ir_div_int),
    '-': (2, ir_sub_int),
    'return': (1, ir_return_fun),
    'Int': (1, ir_make_int),
    'variable': (2, ir_make_variable),
    'lambda': (n, ir_make_lambda),
    'struct': (n, ir_make_struct),
    'union': (n, ir_make_union),
    'Float': (0, ir.FloatType())
}


class LLVMCodeGen(Visitor):
    def __init__(self, module: ir.Module, scope=Scope(), builder=ir):
        super().__init__()
        self.module = module
        self.builder = builder
        self.scope = scope
        self.parent = None
        self.mode = 'codegen'
        self.binding_name = None
        self.count = 0

    def generate_unique_binding_name(self):
        # follow scheme generated name format, identifier starting with # cannot be created by the user
        # which means the name is guaranteed to be unique
        self.count += 1
        return '#id{}'.format(self.count)

    def visit(self, a: Expression, depth=0, mode=None) -> Any:
        if a is not None:

            if mode is not None:
                old = self.mode
                self.mode = mode

            r = a.visit(self, depth)

            if mode is not None:
                self.mode = old

            return r

    # Typed Expression
    def variable(self, var: Variable, depth=0) -> Any:
        v = self.builder.NamedValue(parent=None, name=var.name, type=self.visit(var.type, depth + 1))
        self.scope.insert_binding(var.name, v)

        return v

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'builtin')

        if builtin.name not in builtins:
            raise UndefinedBuiltin(builtin)

        nargs, impl = builtins[builtin.name]

        if self.mode == 'type':
            return impl

        return builtin

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')
        return self.builder.Constant(constant=val.value, typ=self.visit(val.type, depth + 1))

    def struct(self, struct: Struct, depth=0) -> Any:
        return self.builder.LiteralStructType(elems=[self.visit(m, depth + 1) for m in struct.members])

    def union(self, union: Union, depth=0) -> Any:
        # FIXME: build n Tagged LiteralStruct
        return self.builder.LiteralStructType(elems=[self.visit(m, depth + 1) for m in union.members])

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        trace(depth, 'arrow')
        self.scope = self.scope.enter_scope('arrow')

        ftype = self.builder.FunctionType(
            self.visit(arrow.return_type, depth + 1),
            [self.visit(arg, depth + 1) for arg in arrow.args])

        self.scope = self.scope.exit_scope()
        return ftype

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference `{}`'.format(ref.name))

        # Option 1, check if this is a LLVM ref and do not visit
        v = self.scope.get_expression(ref, depth + 1)

        if isinstance(v, Expression):
            return self.visit(v, depth + 1)

        # print(v)
        return v

    # Those should be resolved at eval
    def bind(self, bind: Bind, depth=0) -> Any:
        trace(depth, 'bind')
        name = self.binding_name
        self.binding_name = bind.name
        expr = self.visit(bind.expr, depth + 1)
        self.binding_name = name
        return Bind(bind.name, expr)

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function')

        self.scope = self.scope.enter_scope('function')
        ftype = self.visit(fun.type, depth + 1, mode='type')

        if self.binding_name is None:
            name = self.generate_unique_binding_name()
            self.scope.insert_binding(name, fun)
        else:
            name = self.binding_name

        ir_fun = self.builder.Function(
            self.module,
            ftype=ftype,
            name=name
        )

        args = ir_fun.args

        for arg, larg in zip(fun.args, args):
            self.scope.insert_binding(arg.name, larg)

        #self.scope.dump()

        block = ir_fun.append_basic_block(name='fun_block')
        old = self.builder
        old_p = self.parent
        self.parent = ir_fun

        self.builder = ir.IRBuilder(block)
        self.visit(fun.body, depth + 1)

        self.builder = old
        self.parent = old_p

        self.scope = self.scope.exit_scope()
        return ir_fun

    def block(self, block: Block, depth=0) -> Any:
        trace(depth, 'block')
        blk = ir.Block(self.parent)

        old = self.builder
        old_p = self.parent
        self.builder = ir.IRBuilder(block)

        for expr in block.expressions:
            self.visit(expr, depth + 1)

        self.builder = old
        self.parent = old_p
        return blk

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplementedError

    def conditional(self, cond: Conditional, depth=0) -> Any:
        raise NotImplementedError

    def call(self, call: Call, depth=0) -> Any:
        trace(depth, 'call')

        lcall = self.visit(call.function, depth + 1)
        args = [self.visit(expr, depth + 1) for expr in call.args]

        if isinstance(lcall, Builtin):
            if lcall.name not in builtins:
                raise UndefinedBuiltin(lcall)

            nargs, impl = builtins[lcall.name]

            if nargs != len(call.args):
                raise ArgumentSizeMismatch(nargs, call.args)

            return impl(self.builder, args)

        print(call.function, ' => ', lcall)
        print(args)

        return self.builder.call(lcall, args)

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        trace(depth, 'binary_call')
        return self.call(call, depth + 1)

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        trace(depth, 'unary_call')
        return self.call(call, depth + 1)


def llvm_codegen(expr):
    pass
