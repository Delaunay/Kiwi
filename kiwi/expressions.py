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

# -------------------------


class Expression:
    parent = None

    def __init__(self):
        pass

    def visit(self, visitor: 'Visitor', depth=0):
        raise NotImplemented

    def set_parent(self, parent):
        self.parent = parent
        return self


@dataclass
class Value(Expression, TreeLeaf):
    value: Any
    type: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.value(self, depth)


@dataclass
class Variable(Expression, TreeLeaf):
    name: str
    type: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.variable(self, depth)

    def __repr__(self):
        return 'Var(name={}, type={})'.format(self.name, self.type)


@dataclass
class VariableRef(Expression, TreeLeaf):
    index: int
    size: int
    name: str
    reference: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.reference(self, depth)

    def __repr__(self):
        return 'Ref(index={}, size={}, name={})'.format(self.index, self.size, self.name)

    def reverse_index(self):
        return self.size - self.index


@dataclass
class Function(Expression, TreeBranch):
    args: List[Variable] = field(default_factory=list)
    return_type: Expression = None
    body: Expression = None
    is_lambda: bool = False
    # Not all function can be inlined
    inline: bool = False
    # recursive: bool = True

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.function(self, depth)

    def __repr__(self):
        return """Fun(args={}) -> {}:\n    {}""".format(self.args, self.return_type, self.body)


@dataclass
class Block(Expression, TreeBranch):
    expressions: List[Expression]

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.block(self, depth)

@dataclass
class Match(Expression, TreeBranch):
    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.match(self, depth)


@dataclass
class Conditional(Expression, TreeBranch):
    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.conditional(self, depth)


# Types
class GenericType(Expression):
    pass


@dataclass
class Builtin(Expression, TreeLeaf):
    name: str
    type: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.builtin(self, depth)


@dataclass
class Arrow(GenericType, TreeBranch):
    args: List[Expression]
    return_type: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.arrow(self, depth)


# Function Calls
@dataclass
class Call(Expression, TreeBranch):
    function: Expression
    args: List[Expression]

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.call(self, depth)


class BinaryOperator(Call):
    def __init__(self, fun, lhs, rhs):
        super(Call).__init__()

        self.function = fun
        self.args = [lhs, rhs]

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.binary_operator(self, depth)


class UnaryOperator(Call):
    def __init__(self, fun, expr):
        super(Call).__init__()

        self.function = fun
        self.args = [expr]

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.unary_operator(self, depth)


class Visitor:
    def __init__(self):
        pass

    def visit(self, expr: Expression, depth=0) -> Any:
        if expr is not None:
            return expr.visit(self, depth)

    def value(self, val: Value, depth=0) -> Any:
        raise NotImplemented

    def variable(self, var: Variable, depth=0) -> Any:
        raise NotImplemented

    def reference(self, ref: VariableRef, depth=0) -> Any:
        raise NotImplemented

    def function(self, fun: Function, depth=0) -> Any:
        raise NotImplemented

    def block(self, block: Block, depth=0) -> Any:
        raise NotImplemented

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplemented

    def conditional(self, cond: Conditional, depth=0) -> Any:
        raise NotImplemented

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        raise NotImplemented

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        raise NotImplemented

    def call(self, call: Call, depth=0) -> Any:
        raise NotImplemented

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        raise NotImplemented

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        raise NotImplemented



