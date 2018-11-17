import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.builder import AstBuilder
from kiwi.debug import trace

debug_mode = True
trace = functools.partial(trace, mode=debug_mode, name='[to_string] ')


class ToStringV(Visitor):
    def __init__(self, ctx=Scope()):
        super(ToStringV).__init__()
        # enter a new scope to not affect the original scope
        self.env_stack: Scope = ctx.enter_scope(name='visitor_scope')

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

        self.env_stack = self.env_stack.enter_scope(name='function_scope')

        for arg in fun.args:
            self.env_stack.insert_binding(arg.name, arg)

        args = ', '.join([self.visit(arg, depth + 1) for arg in fun.args])

        if fun.is_lambda:
            fun_str = 'lambda {}: {{ {} }}'.format(
                args, self.visit(fun.body, depth + 1))

            self.env_stack = self.env_stack.exit_scope()
            return fun_str

        #self.env_stack.dump()
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


def to_string(expr, ctx=Scope()):
    return ToStringV.run(expr, ctx)


def print_expr(expr, ctx=Scope()):
    print(ToStringV.run(expr, ctx))


def print_test():
    import sys
    sys.stderr = sys.stdout

    from kiwi.builder import AstBuilder
    from kiwi.builtin import make_scope

    ctx = make_scope()
    builder = AstBuilder(ctx)
    float_type = builder.reference('Float')

    # Make the Add Function
    fun = builder.function()
    fun.args([('x', float_type), ('y', float_type)])
    fun.return_type = float_type

    body = fun.unary_operator(
        fun.reference('return'),
        fun.binary_operator(
            fun.reference('-'),
            fun.reference('x'),
            fun.reference('y')
        )
    )

    fun.body(body)

    fun = fun.make()
    builder.bind('add', fun)
    # Done

    # Make a Call to `add`
    add_fun = builder.reference('add')
    two = builder.call(add_fun, [builder.value(1, float_type), builder.value(2, float_type)])
    # Done

    print('----')
    ctx.dump()
    print('----')

    #print(ctx)
    print('----')
    print(fun)
    print('----')
    print_expr(fun, ctx)
    print('----')
    #print_expr(two, ctx)
    print('----')


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



