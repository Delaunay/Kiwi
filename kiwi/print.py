from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.builder import AstBuilder

class ToStringV(Visitor):
    def __init__(self, ctx = Scope()):
        super(ToStringV).__init__()
        self.env_stack: Scope = ctx

    @staticmethod
    def run(expr: Expression):
        return ToStringV().visit(expr)

    def value(self, val: Value) -> Any:
        return '{}: {}'.format(str(val.value), self.visit(val.type))

    def variable(self, var: Variable) -> Any:
        self.env_stack.insert_binding(var.name, var)
        return '{}'.format(str(var.name))

    def reference(self, ref: VariableRef) -> Any:
        expr = self.env_stack.find_expr(ref.index)
        return self.visit(expr)

    def function(self, fun: Function) -> Any:
        self.env_stack.push_env()
        args = ', '.join([self.visit(arg) for arg in fun.args])

        if fun.is_lambda:
            fun_str = 'lambda {}: {{ {} }}'.format(
                args, self.visit(fun.body))

            self.env_stack.pop_env()
            return fun_str

        fun_str = '({}) -> {} {{ {} }}'.format(
            args,
            self.visit(fun.return_type),
            self.visit(fun.body))

        self.env_stack.pop_env()
        return fun_str

    def match(self, match: Match) -> Any:
        raise NotImplemented

    def conditional(self, cond: Conditional) -> Any:
        raise NotImplemented

    def builtin(self, builtin: Builtin) -> Any:
        print(builtin)
        return '{}'.format(builtin.name)

    def arrow(self, arrow: Arrow) -> Any:
        args = ', '.join([self.visit(arg) for arg in arrow.args])
        return '({}) -> {}'.format(args, self.visit(arrow.return_type))

    def call(self, call: Call) -> Any:
        args = ','.join([self.visit(arg) for arg in call.args])
        return '{}({})'.format(self.visit(call.function), args)

    def binary_operator(self, call: BinaryOperator) -> Any:
        print(call)
        return '{} {} {}'.format(self.visit(call.args[0]), self.visit(call.function), self.visit(call.args[1]))

    def unary_operator(self, call: UnaryOperator) -> Any:
        print(call)
        return '{}({})'.format(self.visit(call.function), self.visit(call.args[0]))


def print_expr(expr):
    print(ToStringV().run(expr))


def print_test():
    ctx = Scope()
    builder = AstBuilder(ctx)
    builder.bind('Type', Builtin('Type', None))

    type_type = builder.reference('Type')
    builder.bind('Float', Builtin('Float', type_type))

    float_type = builder.reference('Float')

    arrow = builder.arrow()
    arrow.args = [float_type, float_type]
    arrow.return_type = float_type
    add_type = arrow.make()

    builder.bind('+', Builtin('+', add_type))

    fun = builder.function()
    fun.args([('x', float_type), ('x', float_type)])
    fun.return_type = float_type

    fun = fun.make()
    builder.bind('add', fun)

    ctx.dump()

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



