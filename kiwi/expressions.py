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
Name = str
# -------------------------


class Expression:
    """
        Base Node for every Expression in Kiwi.
        All Kiwi expressions have attributes. Attributes are information stored at compile time that can be accessed
        and used to store information.

        This is in order to make meta programming and reflection easier to the end user.
        Fore example, if a user wants to implement auto-diff. We can use attributes to store the differenciated function.

        ```
            def mult(a, b):
                return a * b

            def diff(a: Expression, x: Expression):
                dx = get_attribute(a, 'dx')
                return dx(a, x)

            def dx_mult(a: Expression, b: Expression, x: Expression)):
                return a * diff(b, x) + b * diff(a, x)

            # Library stores implementation of their lib as attributes
            set_attribute(mult, 'dx', dx_mult [, override=False])

            # Users can override  the default diff function if need be
            set_attribute(mult, 'dx', my_dx_mult, override=True)
        ```

        Another example would be to implement a smart Task Scheduler, that would use cost to determine the resource
        needed to run the task. For example in the case of matrix multiplication we might want to reserve a few
        cores to make it run faster and stop other tasks while it is being computed.

        One of the most relevant example I can think of is if someone wants to do an library similar to Eigen where
        code is generated at compile time and the most efficient code is generated in respect to some cost function.
        The cost of the function is computed using meta programming. It required Eigen to write quite a few Type traits
        to store such data. We hopes we can use attributes for that use and avoid code duplication.

        ```
            def mult(x, y):
                pass

            set_attribute(mult, 'cost', 2)


        ```

    """
    parent = None
    attributes: Dict[Name, 'Expression'] = {}

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
class StructValue(Value, TreeLeaf):
    value: List[Value]
    type: Expression


@dataclass
class UnionValue(Value, TreeLeaf):
    name: str
    value: Value
    type: Expression


@dataclass
class Variable(Expression, TreeLeaf):
    """ Placeholder """
    name: Name
    type: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.variable(self, depth)

    def __repr__(self):
        return 'Var(name={}, type={})'.format(self.name, self.type)


@dataclass
class Bind(Expression, TreeBranch):
    name: Name
    expr: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.bind(self, depth)


# Reference to a bind expression or a Variable
@dataclass
class VariableRef(Expression, TreeLeaf):
    index: int
    size: int
    name: Name
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
    # type = None

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.function(self, depth)

    def __repr__(self):
        return """Fun(args={}) -> {}:\n    {}""".format(self.args, self.return_type, self.body)

    @property
    def type(self):
        return Arrow(list(map(lambda arg: arg.type, self.args)), self.return_type)


@dataclass
class Block(Expression, TreeBranch):
    """
        Sequentially execute expressions, standard FP languages would have nested expression instead,
        similar to the code below.

        ```
            let a = {expr_1} in expr_2
        ```

        This causes issues in terms of style when a depth of nesting is too high.
        Also if everything is an expression you end up with less clear guidelines on how to indent your code.
        so we rather do the below.

        ```
            a = expr_1 \n
            expr_2
        ```
    """
    expressions: List[Expression]

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.block(self, depth)


class Pattern:
    pass


@dataclass
class ExpressionPattern(Pattern):
    """
        Match expression using the == operator
    """
    expr: Expression


@dataclass
class ConstructorPattern(Pattern):
    """
        Match tagged Union
    """
    name: str
    args: List[Pattern]


@dataclass
class NamePattern(Pattern):
    name: str


@dataclass
class Match(Expression, TreeBranch):
    """
        Used for branching (if elif), dispatch (tagged union), unpacking (extract values from struct)
    """
    target: Expression
    branches: [(Pattern, Expression)]
    default: Expression = None

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.match(self, depth)


# Attributes
# Properties
# Method
# Member
@dataclass
class Union(Expression, TreeBranch):
    """ Tagged union type, which makes it safe and usable in a match expression"""
    members: List[Variable]
    # TODO
    # Make each union value capable of holding every single type without re allocation
    # byte size = max(byte_size(members)) + tag_size
    padded: bool = False

    # Add a Tag to every Union types
    tagged: bool = True
    # How the struct is tagged (str = unique)
    # tag mode (str,  i8, i16, i32, i64)
    tag_mode: str = 'i32'

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.union(self, depth)

    @property
    def type(self):
        return Arrow(self.members, self)


@dataclass
class Struct(Expression, TreeBranch):
    """ Standard C struct, supports unpacking using a match expression
        Additionally, a Kiwi Struct is not distinguishable from a Tuple, and Tuples must have named members.
        This is so
    """
    members: List[Variable]

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.struct(self, depth)

    @property
    def type(self):
        return Arrow(self.members, self)


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
    args: List[Variable]
    return_type: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        return visitor.arrow(self, depth)

    @property
    def compile_time(self):
        from kiwi.builtin import type_type
        from kiwi.operations.equality import kequal

        for arg in self.args:
            if not kequal(arg.type, type_type):
                return False
        return True


@dataclass
class NamedArgument(Expression, TreeBranch):
    name: str
    expr: Expression

    def visit(self, visitor: 'Visitor', depth=0):
        # pass through
        return self.expr.visit(visitor, depth)

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
        raise NotImplementedError

    def bind(self, bind: Bind, depth=0) -> Any:
        raise NotImplementedError

    def variable(self, var: Variable, depth=0) -> Any:
        raise NotImplementedError

    def reference(self, ref: VariableRef, depth=0) -> Any:
        raise NotImplementedError

    def function(self, fun: Function, depth=0) -> Any:
        raise NotImplementedError

    def block(self, block: Block, depth=0) -> Any:
        raise NotImplementedError

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplementedError

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        raise NotImplementedError

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        raise NotImplementedError

    def call(self, call: Call, depth=0) -> Any:
        raise NotImplementedError

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        raise NotImplementedError

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        raise NotImplementedError

    def struct(self, struct: Struct, depth=0) -> Any:
        raise NotImplementedError

    def union(self, union: Union, depth=0) -> Any:
        raise NotImplementedError




