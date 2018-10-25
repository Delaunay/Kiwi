from typing import *
from dataclasses import dataclass
from dataclasses import field

# Tree Generic
# ------------------------
class Tree:
    pass


class TreeLeaf(Tree):
    pass


class TreeBranch(Tree):
    pass
#-------------------------


class Expression:
    def __init__(self):
        pass

    def visit(self, visitor: 'Visitor'):
        raise NotImplemented

@dataclass
class Value(Expression, TreeLeaf):
    value: Any
    type: Expression

    def visit(self, visitor: 'Visitor'):
        return visitor.value(self)


@dataclass
class Variable(Expression, TreeLeaf):
    name: str
    type: Expression

    def visit(self, visitor: 'Visitor'):
        return visitor.variable(self)


@dataclass
class VariableRef(Expression, TreeLeaf):
    index: int

    def visit(self, visitor: 'Visitor'):
        return visitor.reference(self)


@dataclass
class Function(Expression, TreeBranch):
    args: List[Variable] = field(default_factory=list)
    return_type: Expression = None
    body: Expression = None
    is_lambda: bool = False

    def visit(self, visitor: 'Visitor'):
        return visitor.function(self)


@dataclass
class Block(Expression, TreeBranch):
    expressions: List[Expression]

    def visit(self, visitor: 'Visitor'):
        return visitor.block(self)

@dataclass
class Match(Expression, TreeBranch):
    def visit(self, visitor: 'Visitor'):
        return visitor.match(self)


@dataclass
class Conditional(Expression, TreeBranch):
    def visit(self, visitor: 'Visitor'):
        return visitor.conditional(self)

# Types
class GenericType(Expression):
    pass


@dataclass
class Builtin(Expression, TreeLeaf):
    name: str
    type: Expression

    def visit(self, visitor: 'Visitor'):
        return visitor.builtin(self)


@dataclass
class Arrow(GenericType, TreeBranch):
    args: List[Expression]
    return_type: Expression

    def visit(self, visitor: 'Visitor'):
        return visitor.arrow(self)


# Function Calls
@dataclass
class Call(Expression, TreeBranch):
    function: Expression
    args: List[Expression]

    def visit(self, visitor: 'Visitor'):
        return visitor.call(self)


class BinaryOperator(Call):
    def __init__(self, fun, lhs, rhs):
        super(Call).__init__()

        self.function = fun
        self.args = [lhs, rhs]

    def visit(self, visitor: 'Visitor'):
        return visitor.binary_operator(self)


class UnaryOperator(Call):
    def __init__(self, fun, expr):
        super(Call).__init__()

        self.function = fun
        self.args = [expr]

    def visit(self, visitor: 'Visitor'):
        return visitor.unary_operator(self)


class Visitor:
    def __init__(self):
        pass

    def visit(self, expr: Expression) -> Any:
        if expr is not None:
            return expr.visit(self)

    @staticmethod
    def run(expr: Expression):
        return Visitor().visit(expr)

    def value(self, val: Value) -> Any:
        raise NotImplemented

    def variable(self, var: Variable) -> Any:
        raise NotImplemented

    def reference(self, ref: VariableRef) -> Any:
        raise NotImplemented

    def function(self, fun: Function) -> Any:
        raise NotImplemented

    def block(self, block: Block) -> Any:
        raise NotImplemented

    def match(self, match: Match) -> Any:
        raise NotImplemented

    def conditional(self, cond: Conditional) -> Any:
        raise NotImplemented

    def builtin(self, builtin: Builtin) -> Any:
        raise NotImplemented

    def arrow(self, arrow: Arrow) -> Any:
        raise NotImplemented

    def call(self, call: Call) -> Any:
        raise NotImplemented

    def binary_operator(self, call: BinaryOperator) -> Any:
        raise NotImplemented

    def unary_operator(self, call: UnaryOperator) -> Any:
        raise NotImplemented



