import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace
from kiwi.builtin import builtins_implementation
from kiwi.builtin import type_type
from kiwi.type.equality import kequiv

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
        return bind.expr, None

    def variable(self, var: Variable, depth=0) -> Any:
        return var, var.type

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference: {}'.format(ref))
        v = self.scope.get_expression(ref, depth + 1)
        return self.visit(v, depth + 1)

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function: {}'.format(fun.args))
        # function are not evaluated. The evaluation part happens at the call site

        # Fake Call site
        self.scope = self.scope.enter_scope('function')

        for arg in fun.args:
            self.scope.insert_binding(arg.name, arg)

        # Arg types need to be decuced from the body
        print(fun.body)
        a, type = self.visit(fun.body, depth + 1)
        print(a)

        self.scope = self.scope.exit_scope()
        fun.return_type = type
        return fun

    def block(self, block: Block, depth=0) -> Any:
        trace(depth, 'block')
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
                if arg_var.type is None:
                    arg_var.type = type


                # insert the arg to the function scope
                self.scope.insert_binding(arg_var.name, expr)

            print('wuuuut')
            # Evaluate function body
            expr, type = self.visit(fun.body, depth + 1)
            fun.return_type = type

            self.scope = self.scope.exit_scope()
            return expr, type

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

                # fun.type.args will have all the information necessary for inference
                # call.args can be references in which case we need to add the type of not available
                for bargs, cargs in zip(fun.type.args, call.args):

                    if isinstance(cargs, VariableRef):
                        var = self.scope.get_expression(cargs)

                        if var.type is None:
                            var.type = bargs.type
                        else:
                            assert kequiv(var.type, bargs.type)

                # Evaluate every args
                args = [self.visit(arg_expr, depth + 1) for arg_expr in call.args]

                # Evaluate function body
                # return impl(args)
                return fun, arrow.return_type

            return fun, fun.type


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

