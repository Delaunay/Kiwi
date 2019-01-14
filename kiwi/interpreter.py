import functools

from kiwi.operations.equality import kequal
from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace
from kiwi.builtin import builtins_implementation

debug_mode = True
trace = functools.partial(trace, mode=debug_mode, name='[interpreter] ')


def trace_raise(depth, error):
    trace(depth, '/!\\ {}'.format(error))
    raise RuntimeError(error)


class Interpreter(Visitor):
    _builtins = builtins_implementation()

    def __init__(self, scope=Scope()):
        super().__init__()
        self.scope = scope

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value: {}'.format(val))
        return val

    def bind(self, bind: Bind, depth=0) -> Any:
        expr = self.visit(bind.expr, depth + 1)
        self.scope.insert_binding(bind.name, expr)
        var = VariableRef(len(self.scope), -1, bind.name, expr)
        return var

    def variable(self, var: Variable, depth=0) -> Any:
        pass

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference: {}'.format(ref))
        v = self.scope.get_expression(ref, depth + 1)
        return v # self.visit(v, depth + 1)

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function: {}'.format(fun.args))
        # function are not evaluated. The evaluation part happens at the call site
        return fun

    def block(self, block: Block, depth=0) -> Any:
        for expression in block.expressions:
            self.visit(expression, depth + 1)

    def match(self, match: Match, depth=0) -> Any:
        trace(depth, 'match')
        target = self.visit(match.target, depth + 1)

        for pattern, branch in match.branches:

            if isinstance(pattern, ExpressionPattern):
                val = self.visit(pattern.expr, depth + 1)

                if kequal(val, target):
                    return self.visit(branch, depth + 1)

            elif isinstance(pattern, ConstructorPattern):
                self.scope = self.scope.enter_scope('match_unpacking')

                # type = self.visit(pattern.name, depth + 1)

                assert isinstance(target, (StructValue, UnionValue))

                tvalue = target.value
                if isinstance(target.value, Expression):
                    tvalue = [target.value]

                for pat, arg in zip(pattern.args, tvalue):

                    if isinstance(pat, NamePattern):
                        self.scope.insert_binding(pat.name, arg)

                result = self.visit(branch, depth + 1)
                self.scope = self.scope.exit_scope()
                return result

                trace_raise(depth, 'NotImplementedError')

        if match.default is None:
            trace_raise(depth, 'Error No Matching Branch')

        return self.visit(match.default, depth + 1)

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'builtin: {}'.format(builtin.name))
        # builtin are not evaluated. The evaluation part happens at the call site
        return builtin

    def is_type(self, obj):
        return isinstance(obj, (Struct, Union))

    def call(self, call: Call, depth=0) -> Any:
        trace(depth, 'call {}'.format(call))
        fun = self.visit(call.function, depth + 1)

        # Builtin Call
        if isinstance(fun, Builtin):
            return self.builtin_call(fun, call, depth)

        # User defined Function
        if isinstance(fun, Function):
            if len(call.args) != len(fun.args):
                trace_raise('Number of argument mismatch {}: {} != {} : {}'.format(
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

        # This is a constructor call
        if isinstance(fun, Struct):
            # could it be useful to have an option to delay eval here ?
            values = list(map(lambda x: self.visit(x, depth + 1), call.args))
            return StructValue(values, call.function)

        if isinstance(fun, Union):
            assert len(call.args) == 1
            assert isinstance(call.args[0], NamedArgument)

            value = self.visit(call.args[0], depth + 1)
            return UnionValue(name=call.args[0].name, value=value, type=call.function)

        trace_raise('Call `{}` Type is not handled'.format(fun))

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
                trace_raise('Number of argument mismatch')

            # Evaluate every args
            args = [self.visit(arg_expr, depth + 1) for arg_expr in call.args]

            # Evaluate function body
            return impl(args)


def keval(expr, scope=Scope()):
    interpreter = Interpreter(scope)
    return interpreter.visit(expr)
