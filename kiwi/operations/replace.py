from kiwi.expressions import *
from kiwi.operations.equality import kequal


class Replace(Visitor):
    """
        Replace an Expression A by B
        The returned version is a copy
    """
    def __init__(self, target: Expression, replace: Expression):
        self.target = target
        self.replace_expr = replace

    def replace(self, expr):
        if kequal(expr, self.target):
            return self.replace_expr
        return expr

    def visit(self, expr: Expression, depth=0) -> Any:
        if expr is not None:
            return expr.visit(self, depth)

    def value(self, val: Value, depth=0) -> Any:
        if kequal(val, self.target):
            return self.replace_expr
        return val

    def bind(self, bind: Bind, depth=0) -> Any:
        if kequal(bind, self.target):
            return self.replace_expr
        return bind

    def variable(self, var: Variable, depth=0) -> Any:
        if kequal(var, self.target):
            return self.replace_expr

        return Variable(var.name, self.visit(var.type, depth + 1))

    def reference(self, ref: VariableRef, depth=0) -> Any:
        if kequal(ref, self.target):
            return self.replace_expr
        return ref

    def function(self, fun: Function, depth=0) -> Any:
        if kequal(fun, self.target):
            return self.replace_expr
        return fun
    
    def block(self, block: Block, depth=0) -> Any:
        if kequal(block, self.target):
            return self.replace_expr
        return block

    def match(self, match: Match, depth=0) -> Any:
        if kequal(match, self.target):
            return self.replace_expr
        return match

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        if kequal(builtin, self.target):
            return self.replace_expr
        return builtin

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        if kequal(arrow, self.target):
            return self.replace_expr

        return Arrow(
            list(map(lambda e: self.visit(e, depth + 1), arrow.args)),
            self.visit(arrow.return_type, depth + 1)
        )

    def call(self, call: Call, depth=0) -> Any:
        if kequal(call, self.target):
            return self.replace_expr

        return Call(
            self.visit(call.function, depth + 1),
            list(map(lambda e: self.visit(e, depth + 1), call.args))
        )

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        if kequal(call, self.target):
            return self.replace_expr

        return BinaryOperator(
            self.visit(call.function, depth + 1),
            self.visit(call.args[0], depth + 1),
            self.visit(call.args[1], depth + 1)
        )

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        if kequal(call, self.target):
            return self.replace_expr

        return UnaryOperator(
            self.visit(call.function, depth + 1),
            self.visit(call.args[0], depth + 1)
        )

    def struct(self, struct: Struct, depth=0) -> Any:
        if kequal(struct, self.target):
            return self.replace_expr
        new_members = [] * len(struct.members)

        for i, v in enumerate(struct.members):
            new_members[i] = Variable(v.name, self.visit(v.type, depth + 1))

        return Struct(new_members)

    def union(self, union: Union, depth=0) -> Any:
        if kequal(union, self.target):
            return self.replace_expr

        new_members = [] * len(union.members)

        for i, v in enumerate(union.members):
            new_members[i] = Variable(v.name, self.visit(v.type, depth + 1))

        return Union(new_members)


def kreplace(target, rep, source, depth=0):
    rep = Replace(target, rep)
    return rep.visit(source, depth)
