import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.builder import AstBuilder
from kiwi.debug import trace

debug_mode = False
trace = functools.partial(trace, mode=debug_mode, name='[to_string] ')


class ToStringV(Visitor):
    def __init__(self, ctx=Scope()):
        super(ToStringV).__init__()
        # enter a new scope to not affect the original scope
        self.env_stack: Scope = ctx.enter_scope(name='visitor_scope')
        self.bind_name = []

    @staticmethod
    def run(expr: Expression, ctx=Scope()):
        return ToStringV(ctx).visit(expr, depth=0)

    def resolve_reference(self, ref):
        try:
            if isinstance(ref, VariableRef):
                return self.env_stack.get_expression(ref)
        except:
            pass

        return ref

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')

        type = self.resolve_reference(val.type)

        if isinstance(val, StructValue):  # isinstance(type, Struct):
            values = map(lambda x: self.visit(x, depth + 1), val.value)
            return '{}({})'.format(self.visit(val.type, depth + 1), ', '.join(values))

        if isinstance(val, UnionValue):  # isinstance(type, Union):
            return '{}.{}({})'.format(
                self.visit(val.type, depth + 1),
                val.name,
                self.visit(val.value, depth + 1)
            )

        return '{}: {}'.format(str(val.value), self.visit(val.type, depth + 1))

    def variable(self, var: Variable, depth=0) -> Any:
        trace(depth, 'variable {}'.format(var))
        return '{}: {}'.format(str(var.name), self.visit(var.type, depth + 1))

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference {}'.format(ref))
        return ref.name
        # """
        # refs = self.visit(ref.reference, depth + 1)
        # # --------------------------------------------------------------
        # # Sanity Check
        # try:
        #     env_ref = self.env_stack.get_expression(ref, depth)
        #     # name = self.env_stack.get_name(ref)
        #     if ref.reference is not env_ref:
        #         print(ref)
        #         print('Error \n\t{} \n\t\t!=\n\t{}'.format(ref.reference, env_ref))
        # except Exception as e:
        #     print(e)
        # # --------------------------------------------------------------
        # return refs"""

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function {}'.format(''))

        if len(self.bind_name) == 0:
            return self.function_lambda(fun, depth)

        return self.function_bind(self.bind_name[-1], fun, depth)

    def function_lambda(self, fun: Function, depth=0) -> Any:
        self.env_stack = self.env_stack.enter_scope(name='function_scope')

        for arg in fun.args:
            self.env_stack.insert_binding(arg.name, arg)

        args = ', '.join([self.visit(arg, depth + 1) for arg in fun.args])

        if fun.is_lambda:
            fun_str = 'lambda {}: {{ {} }}'.format(
                args, self.visit(fun.body, depth + 1))

            self.env_stack = self.env_stack.exit_scope()
            return fun_str

        # self.env_stack.dump()
        fun_str = '({}) -> {} {{ {} }}'.format(
            args,
            self.visit(fun.return_type, depth + 1),
            self.visit(fun.body, depth + 1))

        self.env_stack = self.env_stack.exit_scope()
        return fun_str

    def function_bind(self, name, fun: Function, depth=0) -> str:

        self.env_stack = self.env_stack.enter_scope(name='function_scope')

        for arg in fun.args:
            self.env_stack.insert_binding(arg.name, arg)

        args = ', '.join([self.visit(arg, depth + 1) for arg in fun.args])
        return_type = self.visit(fun.return_type, depth + 1)

        body = self.visit(fun.body, depth + 1)

        fun_str = 'def {}({}) -> {}:\n   {}'.format(name, args, return_type, body)

        return fun_str

    def match(self, match: Match, depth=0) -> Any:

        def render_pattern(pat):
            if isinstance(pat, ExpressionPattern):
                return self.visit(pat.expr, depth + 1)

        target = self.visit(match.target, depth + 1)
        branches = ['{} => {}'.format(render_pattern(p), self.visit(b, depth + 1)) for p, b in match.patterns]

        if match.default is not None:
            branches.append('_ => {}'.format(self.visit(match.default, depth + 1)))

        return '{} match\n {}'.format(target, '\n   '.join(branches))

    def bind(self, bind: Bind, depth=0) -> Any:
        # FIXME nested bind
        self.bind_name.append(bind.name)
        v = self.visit(bind.expr, depth + 1)
        self.bind_name.pop()
        return v

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'Builtin: {}'.format(builtin))
        return '{}'.format(builtin.name)

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        trace(depth, 'arrow')
        args = ', '.join([self.visit(arg, depth + 1) for arg in arrow.args])
        return '({}) -> {}'.format(args, self.visit(arrow.return_type, depth + 1))

    def call(self, call: Call, depth=0) -> Any:
        trace(depth, 'call')
        fun = self.visit(call.function, depth + 1)
        args = ', '.join([self.visit(arg, depth + 1) for arg in call.args])
        return '{}({})'.format(fun, args)

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        trace(depth, 'binary_call')
        return '{} {} {}'.format(
            self.visit(call.args[0], depth + 1),
            self.visit(call.function, depth + 1),
            self.visit(call.args[1], depth + 1))

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        trace(depth, 'unary_call')
        return '{}({})'.format(self.visit(call.function, depth + 1), self.visit(call.args[0], depth + 1))

    def union(self, union: Union, depth=0) -> Any:
        trace(depth, 'union')
        args = ', '.join(['{}: {}'.format(m.name, self.visit(m.type, depth + 1)) for m in union.members])
        return 'Union({})'.format(args)

    def struct(self, union: Struct, depth=0) -> Any:
        trace(depth, 'struct')
        args = ', '.join(['{}: {}'.format(m.name, self.visit(m.type, depth + 1)) for m in union.members])
        return 'Struct({})'.format(args)


def to_string(expr, ctx=Scope()):
    return ToStringV.run(expr, ctx)


def print_expr(expr, ctx=Scope()):
    print(ToStringV.run(expr, ctx))


