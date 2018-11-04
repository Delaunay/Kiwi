import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace

debug_mode = True
trace = functools.partial(trace, mode=debug_mode)


class Interpreter(Visitor):
    _builtins = {
        'Int': (1, lambda args: Value(args[0].value, type=None)),
        '+': (2, lambda args: Value(args[0].value + args[1].value, type=None)),
        'return': (1, lambda args: args[0].value)
    }

    def __init__(self, scope=Scope()):
        super().__init__()
        self.scope = scope

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value: {}'.format(val))
        return val

    def bind(self, bind: Bind, depth=0) -> Any:
        return bind.expr

    def variable(self, var: Variable, depth=0) -> Any:
        pass

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference: {}'.format(ref))
        return self.scope.get_expression(ref, depth + 1)

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function: {}'.format(fun.args))
        # function are not evaluated. The evaluation part happens at the call site
        return fun

    def block(self, block: Block, depth=0) -> Any:
        for expression in block.expressions:
            self.visit(expression, depth + 1)

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplemented

    def conditional(self, cond: Conditional, depth=0) -> Any:
        raise NotImplemented

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'builtin: {}'.format(builtin.name))
        # builtin are not evaluated. The evaluation part happens at the call site
        return builtin

    def call(self, call: Call, depth=0) -> Any:
        trace(depth, 'call {}'.format(call))
        fun = self.visit(call.function, depth + 1)

        # Builtin Call
        if isinstance(fun, Builtin):
            return self.builtin_call(fun, call, depth)

        # User defined Function
        if isinstance(fun, Function):

            if len(call.args) != len(fun.args):
                raise RuntimeError('Number of argument mismatch {}: {} != {} : {}'.format(
                    fun,
                    len(call.args),
                    len(fun.args),
                    call.args))

            # Evaluate every args
            self.scope = self.scope.enter_scope()
            for arg_expr, arg_var in zip(call.args, fun.args):
                val = self.visit(arg_expr, depth + 1)

                # insert the arg to the function scope
                self.scope.insert_binding(arg_var.name, val)

            # Evaluate function body
            returned_val = self.visit(fun.body, depth + 1)
            self.scope = self.scope.exit_scope()
            return returned_val

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        return self.call(call, depth)

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        return self.call(call, depth)

    def struct(self, struct: Struct, depth=0) -> Any:
        raise NotImplemented

    def union(self, union: Union, depth=0) -> Any:
        raise NotImplemented

    # Type Expression are not evaluated
    def arrow(self, arrow: Arrow, depth=0) -> Any:
        raise NotImplemented

    def builtin_call(self, fun: Builtin, call: Call, depth) -> Any:
        if fun.name in self._builtins:
            nargs, impl = self._builtins[fun.name]

            if len(call.args) != nargs:
                raise RuntimeError('Number of argument mismatch')

            # Evaluate every args
            args = [self.visit(arg_expr, depth + 1) for arg_expr in call.args]

            # Evaluate function body
            return impl(args)


def keval(expr, scope=Scope()):
    interpreter = Interpreter(scope)
    return interpreter.visit(expr)


if __name__ == '__main__':
    import sys
    sys.stderr = sys.stdout

    from kiwi.print import to_string
    from kiwi.builder import AstBuilder

    ctx = Scope()
    builder = AstBuilder(ctx)
    builder.bind('Type', Builtin('Type', None))

    type_type = builder.reference('Type')
    builder.bind('Float', Builtin('Float', type_type))

    float_type = builder.reference('Float')
    return_op = builder.builtin('return', Arrow([type_type], type_type))

    arrow = builder.arrow()
    arrow.args([float_type, float_type])
    arrow.return_type(float_type)
    add_type = arrow.make()

    builder.bind('+', Builtin('+', add_type))

    fun = builder.function()
    fun.args([('x', float_type), ('y', float_type)])
    fun.return_type = float_type

    body = fun.unary_operator(
        fun.reference('return'),
        fun.binary_operator(
            fun.reference('+'),
            fun.reference('x'),
            fun.reference('y')
        )
    )

    fun.body(body)

    fun = fun.make()
    builder.bind('add', fun)
    builder.bind('Integer', Builtin('Integer', type_type))

    add_fun = builder.reference('add')
    two = builder.call(add_fun, [builder.value(1, float_type), builder.value(2, float_type)])

    ctx.dump()

    fun_str = to_string(two, ctx)
    print('-' * 80)
    print(fun_str, '==' , keval(two, ctx))
    print('-' * 80)

