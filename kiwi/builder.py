"""
    We need builders because the variable ref needs to use the local context
"""

from kiwi.expressions import *
from kiwi.environment import Scope


class AstBuilder:
    def __init__(self, scope=Scope()):
        self.scope: Scope = scope
        self.parent = None

    def value(self, val: Any, etype: Expression) -> Value:
        return Value(val, etype).set_parent(self.parent)

    def variable(self, name: str, etype: Expression) -> Variable:
        var = Variable(name, etype)
        self.scope.insert_binding(name, var)
        return var.set_parent(self.parent)

    def reference(self, name: str) -> VariableRef:
        index, expr = self.scope.get_index(name)
        size = len(self.scope)
        return VariableRef(index, size, name, reference=expr).set_parent(self.parent)

    def function(self) -> 'FunctionBuilder':
        return FunctionBuilder(self.scope.enter_scope(), parent=self.parent)

    def block(self) -> 'BlockBuilder':
        return BlockBuilder(self.scope.enter_scope(), parent=self.parent)

    def match(self) -> 'MatchBuilder':
        return MatchBuilder(self.scope.enter_scope(), parent=self.parent)

    def bind(self, name: str, expr: Expression):
        self.scope.insert_binding(name, expr)
        return Bind(name, expr)

    def builtin(self, name: str, etype: Expression) -> Builtin:
        b = Builtin(name, etype)
        self.scope.insert_binding(name, b)
        return b.set_parent(self.parent)

    def arrow(self) -> 'ArrowBuilder':
        return ArrowBuilder(self.scope.enter_scope(), parent=self.parent)

    def call(self, fun: Expression, args: List[Expression]) -> Call:
        return Call(fun, args).set_parent(self.parent)

    def binary_operator(self, fun: Expression, lhs: Expression, rhs: Expression) -> BinaryOperator:
        return BinaryOperator(fun, lhs, rhs).set_parent(self.parent)

    def unary_operator(self, fun: Expression, expr: Expression) -> UnaryOperator:
        return UnaryOperator(fun, expr).set_parent(self.parent)

    def union(self):
        return UnionBuilder(self.scope.enter_scope(), parent=self.parent)

    def struct(self):
        return StructBuilder(self.scope.enter_scope(), parent=self.parent)

    def named_arg(self, name:str, expr: Expression) -> NamedArgument:
        return NamedArgument(name, expr)


class FunctionBuilder(AstBuilder):
    def __init__(self, context, parent):
        super().__init__(context)
        self.parent = Function().set_parent(parent)
        self.fun = self.parent

    def args(self, args: List[Tuple[str, Expression]]):
        self.fun.args = [self.variable(name, etype) for name, etype in args]
        return self

    def return_type(self, expr):
        self.fun.return_type = expr
        return self

    def body(self, body):
        self.fun.body = body

    def make(self):
        return self.fun


class BlockBuilder(AstBuilder):
    def __init__(self, context, parent):
        super().__init__(context)


class MatchBuilder(AstBuilder):
    def __init__(self, context, parent):
        super().__init__(context)
        self.match = Match(None, [], None)

    def target(self, expr):
        self.match.target = expr

    def add_expr_branch(self, expr, branch):
        self.match.branches.append((ExpressionPattern(expr), branch))

    def add_branch(self, pattern, branch):
        self.match.branches.append((pattern, branch))

    def add_unpacking(self, ctor: Expression, args: List[str], branch):
        pattern = ConstructorPattern(ctor, list(map(lambda x: NamePattern(x), args)))
        self.match.branches.append((pattern, branch))

    def make(self):
        return self.match


class ArrowBuilder(AstBuilder):
    def __init__(self, context, parent):
        super().__init__(context)
        self.parent = Arrow([], None).set_parent(parent)
        self.arrow = self.parent

    def args(self, args: List[Expression]):
        self.arrow.args = args
        return self

    def return_type(self, expr):
        self.arrow.return_type = expr
        return self

    def make(self):
        return self.arrow


class UnionBuilder(AstBuilder):
    def __init__(self, context, parent):
        super().__init__(context)
        self.parent = Union([]).set_parent(parent)
        self.union = self.parent

    def members(self, members: List[Expression]):
        self.union.members = members

    def make(self):
        return self.union


class StructBuilder(AstBuilder):
    def __init__(self, context, parent):
        super().__init__(context)
        self.parent = Struct([]).set_parent(parent)
        self.struct = self.parent

    def members(self, members: List[Expression]):
        self.struct.members = members

    def make(self):
        return self.struct
