import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace
from kiwi.builtin import builtins_implementation
from kiwi.builtin import type_type
from kiwi.type.equality import ktype_equiv

debug_mode = True
trace = functools.partial(trace, mode=debug_mode, name='[type_trace] ')


class TypeTrace(Visitor):
    """
        Pseudo Interpreter,
    """
    _builtins = builtins_implementation()

    def __init__(self, scope=Scope(), type_hint=None):
        super().__init__()
        self.scope = scope
        self.type_hint = type_hint

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value: {}'.format(val))
        return val, val.type

    def bind(self, bind: Bind, depth=0) -> Any:
        expr, type = self.visit(bind.expr, depth + 1)
        return bind, type

    def variable(self, var: Variable, depth=0) -> Any:
        trace(depth, 'variable: {}'.format(var.name))

        if var.type is None:
            var.type = self.type_hint

        elif self.type_hint is not None:
            assert ktype_equiv(var.type, self.type_hint, self.scope)

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

        # Arg types need to be deduced from the body
        _, type = self.visit(fun.body, depth + 1)

        self.scope = self.scope.exit_scope()
        fun.return_type = type
        return fun, fun.type

    def block(self, block: Block, depth=0) -> Any:
        trace(depth, 'block')

        for expression in block.expressions:
            self.visit(expression, depth + 1)

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplementedError

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'builtin: {}'.format(builtin.name))

        # builtin are not evaluated. The evaluation part happens at the call site
        return builtin, builtin.type

    def call(self, call: Call, depth=0) -> Any:
        trace(depth, 'call {}'.format(call))

        fun, fun_type = self.visit(call.function, depth + 1)

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

                self.type_hint = arg_var.type

                expr, type = self.visit(arg_expr, depth + 1)

                # deduce type from call site
                if arg_var.type is None:
                    arg_var.type = type

                # insert the arg to the function scope
                self.scope.insert_binding(arg_var.name, expr)

            # Skip body eval since it was done when the function call was resolved during the fake call
            # Evaluate function body
            # expr, type = self.visit(fun.body, depth + 1)
            fun.return_type = fun_type.return_type

            self.scope = self.scope.exit_scope()
            return call, fun_type.return_type

        # Constructors
        if isinstance(fun, Struct):

            for sargs, cargs in zip(fun.members, call.args):
                expr, type = self.visit(cargs, depth + 1)
                assert ktype_equiv(sargs.type, type, self.scope)

            return call, fun_type.return_type

        if isinstance(fun, Union):
            assert len(call.args) == 1

            arg = call.args[0]
            assert isinstance(arg, NamedArgument)

            # No need for crazy lookups here the number of members should be low
            union_member = None
            for sargs in fun.members:

                if sargs.name == arg.name:
                    union_member = sargs

            if union_member is None:
                raise RuntimeError('Union Member `{}` does not exist'.format(arg.name))

            expr, type = self.visit(arg, depth + 1)
            assert ktype_equiv(type, union_member.type, self.scope)

            return call, fun_type.return_type

        raise RuntimeError('Call `{}` Type is not handled'.format(fun))

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        return self.call(call, depth)

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        return self.call(call, depth)

    def struct(self, struct: Struct, depth=0) -> Any:
        return struct, struct.type

    def union(self, union: Union, depth=0) -> Any:
        return union, union.type

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

                # Evaluate function body
                return fun, arrow.return_type

            return fun, fun.type


def type_trace(expr, scope=Scope(), depth=0, hint=None):
    tracer = TypeTrace(scope, hint)
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
            expr, type = type_trace(varg, scope, depth + 1, hint=targ.type)
            meta_args[targ.type.name] = type

            # /!\ Awful complexity
            arrow = kreplace(targ.type, type, arrow)
    else:
        for targ, varg in zip(arrow.args, args):
            expr, type = type_trace(varg, scope, depth + 1, hint=targ.type)

            assert ktype_equiv(type, targ.type, scope)

    return arrow

