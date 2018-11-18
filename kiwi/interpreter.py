import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace
from kiwi.builtin import builtins_implementation

debug_mode = True
trace = functools.partial(trace, mode=debug_mode, name='[interpreter] ')


class Interpreter(Visitor):
    _builtins = builtins_implementation()

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
        v = self.scope.get_expression(ref, depth + 1)
        #print(ref.name, v)
        return v

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function: {}'.format(fun.args))
        # function are not evaluated. The evaluation part happens at the call site
        return fun

    def block(self, block: Block, depth=0) -> Any:
        for expression in block.expressions:
            self.visit(expression, depth + 1)

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplementedError

    def conditional(self, cond: Conditional, depth=0) -> Any:
        raise NotImplementedError

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
            self.scope = self.scope.enter_scope(name='call_scope')
            for arg_expr, arg_var in zip(call.args, fun.args):
                val = self.visit(arg_expr, depth + 1)

                # insert the arg to the function scope
                self.scope.insert_binding(arg_var.name, val)

            # Evaluate function body
            returned_val = self.visit(fun.body, depth + 1)

            self.scope = self.scope.exit_scope()
            return returned_val

        raise RuntimeError('Call `{}` Type is not handled'.format(fun))

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        return self.call(call, depth)

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        return self.call(call, depth)

    def struct(self, struct: Struct, depth=0) -> Any:
        return struct

    def union(self, union: Union, depth=0) -> Any:
        return union

    # Type Expression are not evaluated
    def arrow(self, arrow: Arrow, depth=0) -> Any:
        return arrow

    def builtin_call(self, fun: Builtin, call: Call, depth) -> Any:
        if fun.name in self._builtins:
            nargs, impl = self._builtins[fun.name]

            if nargs is not None and len(call.args) != nargs:
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
            fun.reference('+'),
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

    ctx.dump()

    print('-' * 80)
    fun_str = to_string(two, ctx)
    print('-' * 80)
    result = keval(two, ctx)
    print('-' * 80)
    pretty_result = to_string(result, ctx)
    print('-' * 80)
    print(fun_str, '==', pretty_result)
    print('-' * 80)

