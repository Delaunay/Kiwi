from functools import partial
from functools import reduce

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace

from llvmlite import ir

debug_mode = True
trace = partial(trace, mode=debug_mode, name='[llvm_codegen] ')

n = None


def ir_add_int(builder: ir.IRBuilder, args):
    print('HERE')
    return builder.add(args[0], args[1])


def ir_mult_int(builder: ir.IRBuilder, args):
    return builder.mul(args[0], args[1])


def ir_div_int(builder: ir.IRBuilder, args):
    return builder.sdiv(args[0], args[1])


def ir_sub_int(builder: ir.IRBuilder, args):
    return builder.sub(args[0], args[1])


def ir_return_fun(builder: ir.IRBuilder, args):
    print('HERE')
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
    def __init__(self, module: ir.Module, builder=ir):
        super().__init__()
        self.module = module
        self.variables = {}
        self.builder = builder
        self.parent = None

    def visit(self, a: Expression, depth=0) -> Any:
        if a is not None:
            r = a.visit(self, depth)
            return r

    # Typed Expression
    def variable(self, var: Variable, depth=0) -> Any:
        v = self.builder.NamedValue(parent=None, name=var.name, type=self.visit(var.type, depth + 1))
        self.variables[var.name] = v
        return v

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        if isinstance(builtin, VariableRef):
            nargs, impl = builtins[builtin.name]
            if nargs == 0:
                return impl
        else:
            raise NotImplementedError

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')
        return self.builder.Constant(constant=val.value, typ=self.visit(val.type, depth + 1))

    def struct(self, struct: Struct, depth=0) -> Any:
        return self.builder.LiteralStructType(elems=[self.visit(m, depth + 1) for m in struct.members])

    def union(self, union: Union, depth=0) -> Any:
        return self.builder.LiteralStructType(elems=[self.visit(m, depth + 1) for m in union.members])

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        trace(depth, 'arrow')

        return self.builder.FunctionType(
            self.visit(arrow.return_type, depth + 1),
            [self.visit(arg, depth + 1) for arg in arrow.args])

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference')

        if ref.name in builtins:
            return self.builtin(ref, depth + 1)

        return self.variables[ref.name]

    # Those should be resolved at eval
    def bind(self, bind: Bind, depth=0) -> Any:
        trace(depth, 'bind')
        raise NotImplementedError

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function')

        ir_fun = self.builder.Function(
            self.module,
            ftype=self.visit(fun.type, depth + 1),
            name=None
        )

        args = ir_fun.args

        for arg, larg in zip(fun.args, args):
            self.variables[arg.name] = larg

        block = ir_fun.append_basic_block(name='fun_block')
        old = self.builder
        old_p = self.parent
        self.parent = ir_fun

        self.builder = ir.IRBuilder(block)
        self.visit(fun.body, depth + 1)

        self.builder = old
        self.parent = old_p
        return fun

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

        return self.builder.call(lcall,
                                 [self.visit(expr, depth + 1) for expr in call.args])

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        trace(depth, 'binary_call')
        return self.call(call, depth + 1)

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        trace(depth, 'unary_call')
        return self.call(call, depth + 1)


def llvm_codegen(expr):
    pass
