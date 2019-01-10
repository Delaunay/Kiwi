from functools import partial
from functools import reduce

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace

debug_mode = False
trace = partial(trace, mode=debug_mode, name='[type_check] ')


def type_equal(a: Expression, b: Expression, scope: Scope, depth=0):

    def resolve_ref(ref):
        if isinstance(ref, VariableRef):
            return scope.get_expression(ref)
        return ref

    a = resolve_ref(a)
    b = resolve_ref(b)

    # what if we compare a ref to the refereed expr
    if a.__class__ is not b.__class__:
        return False

    if a is b:
        return True

    return TypeEqual(b, scope).visit(a, depth)


class TypeEqual(Visitor):
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

    def resolve_ref(self, ref):
        if isinstance(ref, VariableRef):
            return self.scope.get_expression(ref)
        return ref

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
        return builtin.name == self.b.name

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')
        return self.type_check(val.type, self.b.type, depth + 1)

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


def ktype_equiv(a: Expression, b: Expression, scope: Scope = Scope(), depth=0) -> bool:
    return type_equal(a, b, scope, depth)


if __name__ == '__main__':
    import sys
    sys.stderr = sys.stdout

    from kiwi.print import to_string
    from kiwi.builder import AstBuilder
    from kiwi.builtin import make_scope

    ctx = make_scope()
    builder = AstBuilder(ctx)

    int_t = builder.reference('Int')
    float_t = builder.reference('Float')

    print(type_equal(builder.value(2, int_t), builder.value(2, float_t), ctx))
    print(type_equal(builder.value(2, float_t), builder.value(2, float_t), ctx))
    arw = builder.arrow()
    arw.args([int_t, int_t])
    arw.return_type(int_t)
    a1 = arw.make()

    arw = builder.arrow()
    arw.args([int_t, int_t])
    arw.return_type(int_t)
    a2 = arw.make()

    arw = builder.arrow()
    arw.args([int_t, int_t])
    arw.return_type(float_t)
    a3 = arw.make()

    print(type_equal(a1, a2, ctx))
    print(type_equal(a1, a3, ctx))
