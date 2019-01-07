import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace
from kiwi.builtin import builtins_implementation
from kiwi.builtin import type_type

debug_mode = True
trace = functools.partial(trace, mode=debug_mode, name='[type_trace] ')


class TypeTrace(Visitor):
    """
        Pseudo Interpreter,
    """
    _builtins = builtins_implementation()

    def __init__(self, scope=Scope()):
        super().__init__()
        self.scope = scope

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value: {}'.format(val))
        return val, val.type

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
                expr, type = self.visit(arg_expr, depth + 1)

                # deduce type from call site
                arg_var.type = type

                # insert the arg to the function scope
                self.scope.insert_binding(arg_var.name, expr)

            # Evaluate function body
            expr, type = self.visit(fun.body, depth + 1)
            fun.return_type = type

            self.scope = self.scope.exit_scope()
            return expr

        raise RuntimeError('Call `{}` Type is not handled'.format(fun))

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        return self.call(call, depth)

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        return self.call(call, depth)

    def struct(self, struct: Struct, depth=0) -> Any:
        return struct, type_type

    def union(self, union: Union, depth=0) -> Any:
        return union, type_type

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        return arrow, type_type

    def builtin_call(self, fun: Builtin, call: Call, depth) -> Any:
        if fun.name in self._builtins:
            nargs, impl = self._builtins[fun.name]

            if nargs is not None and len(call.args) != nargs:
                raise RuntimeError('Number of argument mismatch')

            # Builtin function call
            if isinstance(fun.type, Arrow):
                arrow = resolve_arrow(fun.type, call.args, self.scope, depth)

                #for val, var in zip(call.args, fun.type.args):
                #    print("--", val, var)

                # Evaluate every args
                args = [self.visit(arg_expr, depth + 1) for arg_expr in call.args]

                # Evaluate function body
                # return impl(args)
                return None, arrow.return_type

            print(fun.type)
            return None, fun.type


def type_trace(expr, scope=Scope(), depth=0):
    tracer = TypeTrace(scope)
    return tracer.visit(expr, depth)


def resolve_arrow(arrow, args, scope=Scope(), depth=0):
    from kiwi.operations.replace import kreplace

    # builder.builtin('return', Arrow([make_var('T', 'Type')], Arrow([make_var('r', 'T')], ref('T'))))
    meta_args = {}

    if arrow.compile_time:
        meta = arrow
        arrow = arrow.return_type

        for meta_arg in meta.args:
            meta_args[meta_arg.name] = meta_arg.type

        for targ, varg in zip(arrow.args, args):
            expr, type = type_trace(varg, scope, depth)
            meta_args[targ.type.name] = type

            # /!\ Awful complexity
            arrow = kreplace(targ.type, type, arrow)

    return arrow

