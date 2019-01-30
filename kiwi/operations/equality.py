from functools import partial
from functools import reduce

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace

debug_mode = False
trace = partial(trace, mode=debug_mode, name='[equality_check] ')


def value_equal(a: Expression, b: Expression, scope: Scope, depth=0):
    # what if we compare a ref to the refereed expr
    if a.__class__ is not b.__class__:
        return False

    if a is b:
        return True

    return ValueEqual(b, scope).visit(a, depth)


class ValueEqual(Visitor):
    """
        structural_check: check for structural equality and not

        Point = struct(x: Int, y: Int)
        MyPoint = struct(x :I Int, y: Int)

        Point == MyPoint with structural_check = 1


        iff the expression is canonical `type_check(keval(a), keval(b))`
    """

    def __init__(self, b: Expression, scope: Scope, structural_check=True):
        super().__init__()
        self.scope = scope
        self.structural_check = structural_check
        self.b = b

    def visit(self, a: Expression, depth=0) -> Any:
        if a is not None:
            return a.visit(self, depth)

    def type_check(self, a: Expression, b: Expression, depth):
        return type_equal(a, b, self.scope, depth)

    # Typed Expression
    def variable(self, var: Variable, depth=0) -> Any:
        trace(depth, 'variable')
        return self.type_check(var.type, self.b.type, depth + 1)

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'builtin')
        return self.type_check(builtin.type, self.b.type, depth + 1)

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')
        # print(val.value, self.b.value)

        if isinstance(val.value, Expression):
            self.b = self.b.value
            return self.visit(val.value, depth + 1)

        return val.value == self.b.value

    def struct(self, struct: Struct, depth=0) -> Any:
        trace(depth, 'struct')
        if not self.structural_check:
            return struct is self.b

        if len(struct.members) != len(self.b.members):
            return False

        args = [self.type_check(a.type, b.type, depth + 1) and a.name == b.name
                for a, b in zip(struct.members, self.b.members)]

        return reduce(lambda x, y: x and y, args, True)

    def union(self, union: Union, depth=0) -> Any:
        return self.struct(union, depth)

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        trace(depth, 'arrow')
        if len(arrow.args) != len(self.b.args):
            return False

        args = [self.type_check(a, b, depth + 1) for a, b in zip(arrow.args, self.b.args)]

        return self.type_check(arrow.return_type, self.b.return_type, depth + 1) \
               and reduce(lambda x, y: x and y, args, True)

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference')
        return ref.name == self.b.name

    # Those should be resolved at eval
    def bind(self, bind: Bind, depth=0) -> Any:
        raise NotImplementedError

    def function(self, fun: Function, depth=0) -> Any:
        raise NotImplementedError

    def block(self, block: Block, depth=0) -> Any:
        raise NotImplementedError

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplementedError

    def call(self, call: Call, depth=0) -> Any:
        raise NotImplementedError

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        raise NotImplementedError

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        raise NotImplementedError


def kequal(a: Expression, b: Expression, scope: Scope = Scope(), depth=0) -> bool:
    r = value_equal(a, b, scope, depth)

    #if not r:
    #    print(a)
    #    print(b)
    return r
