import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace
from kiwi.builtin import builtins_implementation
from kiwi.builtin import type_type
from kiwi.type.equality import ktype_equiv
from kiwi.print import to_string

debug_mode = True
trace = functools.partial(trace, mode=debug_mode, name='[type_trace] ')


def trace_raise(depth, val):
    trace(depth, val)
    raise RuntimeError(val)


def trace_assert(depth, pred, msg):
    if not pred:
        trace(depth, msg)
        raise RuntimeError(msg)


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
        trace(depth, 'variable: {}: {}'.format(var.name, var.type))

        if var.type is None:
            var.type = self.type_hint

        elif self.type_hint is not None:
            trace_assert(depth, ktype_equiv(var.type, self.type_hint, self.scope), 'type mismatch')

        return var, var.type    # self.visit(var.type, depth + 1)[0]

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
        trace(depth, 'match')
        target, ttype = self.visit(match.target, depth + 1)

        return_type = None
        for pattern, branch in match.branches:
            self.scope = self.scope.enter_scope('match_branch_scope')

            if isinstance(pattern, ExpressionPattern):
                self.type_hint = ttype
                pat, pat_type = self.visit(pattern.expr, depth + 1)
                self.type_hint = None

                trace_assert(
                    depth,
                    ktype_equiv(pat_type, ttype, self.scope),
                    'Pattern must have the same type as the target {} != {}'.format(
                        to_string(pat_type),
                        to_string(ttype)
                    )
                )

            elif isinstance(pattern, ConstructorPattern):
                target_type, type = self.visit(ttype, depth + 1)
                assert isinstance(target_type, (Union, Struct))

                if pattern.name is not None:
                    # Name should match the struct type
                    if isinstance(pattern.name, Expression):
                        type, type_type = self.visit(pattern.name, depth + 1)

                        #print('HERE1', pattern.name)

                    # Or a member of the union
                    elif isinstance(pattern.name, str):

                        union_member = None
                        for member in target_type.members:
                            if member.name == pattern.name:
                                union_member = member

                        for pat in pattern.args:
                            if isinstance(pat, NamePattern):
                                self.scope.insert_binding(pat.name, Variable(pat.name, union_member.type))

            self.type_hint = return_type
            rbranch, new_return = self.visit(branch, depth + 1)
            self.type_hint = None

            if return_type is not None:
                trace_assert(depth, ktype_equiv(new_return, return_type, self.scope),
                             '/!\\ [TYPE MISMATCH]: Function cannot return two different types `{}` and `{}`'.format(
                                 to_string(new_return, self.scope),
                                 to_string(return_type, self.scope)
                             ))

            return_type = new_return
            self.scope = self.scope.exit_scope()

        if match.default is not None:
            self.type_hint = return_type
            dbranch, dtype = self.visit(match.default, depth + 1)
            self.type_hint = None

            trace_assert(depth, ktype_equiv(dtype, return_type, self.scope), 'type mismatch')

        return match, return_type

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
                trace_raise(depth, 'Number of argument mismatch {}: {} != {} : {}'.format(
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

                trace_assert(depth, ktype_equiv(sargs.type, type, self.scope), "type mismatch")

            return call, fun_type.return_type

        if isinstance(fun, Union):
            trace_assert(depth, len(call.args) == 1, 'Union instantiation takes a single argument')

            arg = call.args[0]
            trace_assert(depth, isinstance(arg, NamedArgument), 'Union takes a single NamedArgument')

            # No need for crazy lookups here the number of members should be low
            union_member = None
            for sargs in fun.members:

                if sargs.name == arg.name:
                    union_member = sargs

            if union_member is None:
                trace_raise(depth, 'Union Member `{}` does not exist'.format(arg.name))

            expr, type = self.visit(arg, depth + 1)

            trace_assert(depth, ktype_equiv(type, union_member.type, self.scope), 'Type mismatch')

            return call, fun_type.return_type

        trace_raise(depth, 'Call `{}` Type is not handled'.format(fun))

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
                trace_raise(depth, 'Number of argument mismatch')

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
            v = type_trace(varg, scope, depth + 1, hint=targ.type)
            # print(varg, v)
            expr, type = v
            meta_args[targ.type.name] = type

            # /!\ Awful complexity
            arrow = kreplace(targ.type, type, arrow)
    else:
        for targ, varg in zip(arrow.args, args):
            expr, type = type_trace(varg, scope, depth + 1, hint=targ.type)

            assert ktype_equiv(type, targ.type, scope)

    return arrow

