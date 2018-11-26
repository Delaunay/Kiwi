from functools import partial
from functools import reduce

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace

debug_mode = True
trace = partial(trace, mode=debug_mode, name='[type_deduce] ')


def type_deduce(a: Expression, scope: Scope, depth = 0) -> Expression:
    return TypeDeduce(scope).visit(a, depth)


class TypeDeduce(Visitor):
    """
        Propagate type information up the AST, this is to reduce the work we have to do during the type inference.
        A lot of type info can be deduced from the leaves.

        We could insert meta types for type that cannot be deduces and those meta types will get resolved during inference

    """
    def __init__(self, scope: Scope):
        super().__init__()
        self.scope = scope

    def visit(self, a: Expression, depth=0) -> Any:
        if a is not None:
            return a.visit(self, depth)

    def type_deduce(self, a: Expression, depth):
        return type_deduce(a, self.scope, depth)

    # Typed expression, just return the type
    def variable(self, var: Variable, depth=0) -> Any:
        trace(depth, 'variable')
        return var.type

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'builtin')
        print(builtin.name)
        return builtin.type

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')
        return val.type

    def struct(self, struct: Struct, depth=0) -> Any:
        trace(depth, 'struct')
        return struct

    def union(self, union: Union, depth=0) -> Any:
        trace(depth, 'union')
        return union

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        trace(depth, 'arrow')
        return arrow

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference')
        return self.type_deduce(self.scope.get_expression(ref, depth), depth + 1)

    # Those should be resolved at eval
    def bind(self, bind: Bind, depth=0) -> Any:
        raise NotImplementedError

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function')
        if fun.type is not None:
            return fun.type

        # might be provided
        rtype = fun.return_type
        if rtype is None:
            rtype = self.type_deduce(fun.body, depth + 1)

        fun.type = Arrow([arg.type for arg in fun.args], rtype)
        return fun.type

    def block(self, block: Block, depth=0) -> Any:
        raise NotImplementedError

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplementedError

    def conditional(self, cond: Conditional, depth=0) -> Any:
        raise NotImplementedError

    def call(self, call: Call, depth=0) -> Any:
        fun_type = self.type_deduce(call.function, depth + 1)

        return fun_type.return_type

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        trace(depth, 'binary')
        fun_type = self.type_deduce(call.function, depth + 1)
        return fun_type.return_type

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        trace(depth, 'unary')
        fun_type = self.type_deduce(call.function, depth + 1)
        return fun_type.return_type
