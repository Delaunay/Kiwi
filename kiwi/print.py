from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.builder import AstBuilder


def trace(depth, message):
    print(''.join(['|' if i % 2 else ':' for i in range(depth)]) + '-> {}'.format(message))


class ToStringV(Visitor):
    def __init__(self, ctx=Scope()):
        super(ToStringV).__init__()
        # enter a new scope to not affect the original scope
        self.env_stack: Scope = ctx.enter_scope()

    @staticmethod
    def run(expr: Expression, ctx=Scope()):
        return ToStringV(ctx).visit(expr, depth=0)

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')
        return '{}: {}'.format(str(val.value), self.visit(val.type, depth + 1))

    def variable(self, var: Variable, depth=0) -> Any:
        trace(depth, 'variable {}'.format(var))
        return '{}'.format(str(var.name))

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'Reference {}'.format(ref))
        return self.visit(ref.reference, depth + 1)

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function')

        self.env_stack = self.env_stack.enter_scope()
        for arg in fun.args:
            self.env_stack.insert_binding(arg.name, arg)

        args = ', '.join([self.visit(arg, depth + 1) for arg in fun.args])

        if fun.is_lambda:
            fun_str = 'lambda {}: {{ {} }}'.format(
                args, self.visit(fun.body, depth + 1))

            self.env_stack = self.env_stack.exit_scope()
            return fun_str

        fun_str = '({}) -> {} {{ {} }}'.format(
            args,
            self.visit(fun.return_type, depth + 1),
            self.visit(fun.body, depth + 1))

        self.env_stack = self.env_stack.exit_scope()
        return fun_str

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplemented

    def conditional(self, cond: Conditional, depth=0) -> Any:
        raise NotImplemented

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'Builtin: {}'.format(builtin))
        return '{}'.format(builtin.name)

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        trace(depth, 'arrow')
        args = ', '.join([self.visit(arg) for arg in arrow.args])
        return '({}) -> {}'.format(args, self.visit(arrow.return_type))

    def call(self, call: Call, depth=0) -> Any:
        trace(depth, 'call')
        args = ','.join([self.visit(arg) for arg in call.args])
        return '{}({})'.format(self.visit(call.function, depth + 1), args)

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        trace(depth, 'binary_call')
        #self.env_stack.dump()
        #print('---')
        return '{} {} {}'.format(
            self.visit(call.args[0], depth + 1),
            self.visit(call.function, depth + 1),
            self.visit(call.args[1], depth + 1))

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        print(' ' * depth, call)
        return '{}({})'.format(self.visit(call.function), self.visit(call.args[0]))


def print_expr(expr, ctx=Scope()):
    print(ToStringV.run(expr, ctx))


def print_test():
    ctx = Scope()
    builder = AstBuilder(ctx)
    builder.bind('Type', Builtin('Type', None))

    type_type = builder.reference('Type')
    builder.bind('Float', Builtin('Float', type_type))

    float_type = builder.reference('Float')

    arrow = builder.arrow()
    arrow.args([float_type, float_type])
    arrow.return_type(float_type)
    add_type = arrow.make()

    builder.bind('+', Builtin('+', add_type))

    fun = builder.function()
    fun.args([('x', float_type), ('y', float_type)])
    fun.return_type = float_type

    body = fun.binary_operator(
        fun.reference('+'),
        fun.reference('x'),
        fun.reference('y')
    )

    fun.body(body)

    fun = fun.make()
    builder.bind('add', fun)
    builder.bind('Integer', Builtin('Integer', type_type))

    print('----')
    ctx.dump()

    print('---')
    print(ctx)
    print('----')
    print(fun)
    print('----')
    print_expr(fun)

"""
    type_type = ctx.insert_binding('Type', Builtin('Type', None))
    ctx.insert_binding('Float', Builtin('Float', type_type))

    return_op = ctx.insert_binding('return', Builtin('return', Arrow([type_type], type_type)))

    add_type = Arrow([float_type, float_type], float_type)
    add_fun = ctx.insert_binding('+', Builtin('+', add_type))

    fun = Function(
        [Variable('x', float_type), Variable('y', float_type)], float_type,
        UnaryOperator(
            return_op,
            BinaryOperator(
                add_fun,
                # Todo remove hardcoded ref
                VariableRef(5),
                VariableRef(6)
            )
        ), is_lambda=True
    )

    a = ctx.insert_binding('add2', fun)
    print(a)
    #val = Value('abc', Builtin('String', ctx.reference('Type')))

    #print(val)
    #print_expr(val)
    print_expr(add_type)

    print(fun)
    print_expr(fun)
    """


if __name__ == '__main__':
    print_test()



