"""
    We need builders because the variable ref needs to use the local context
"""

from kiwi.expressions import *
from kiwi.environment import Scope

class AstBuilder:
    def __init__(self, scope = Scope()):
        self.scope: Scope = scope

    @staticmethod
    def value(val: Any, etype: Expression) -> Value:
        return Value(val, etype)

    def variable(self, name: str, etype: Expression) -> Variable:
        var = Variable(name, etype)
        self.scope.insert_binding(name, var)
        return var

    def reference(self, name: str) -> VariableRef:
        index = self.scope.get_index(name)
        return VariableRef(index)

    def function(self) -> 'FunctionBuilder':
        return FunctionBuilder(self.scope.enter_scope())

    def block(self) -> 'BlockBuilder':
        return BlockBuilder(self.scope.enter_scope())

    def match(self) -> 'MatchBuilder':
        return MatchBuilder(self.scope.enter_scope())


    def bind(self, name: str, expr: Expression):
        self.scope.insert_binding(name, expr)

    #def conditional(self, cond: Conditional) -> Conditional:
    #    raise NotImplemented

    def builtin(self, name: str, etype: Expression) -> Builtin:
        b = Builtin(name, etype)
        self.scope.insert_binding(name, b)
        return b

    def arrow(self) -> 'ArrowBuilder':
        return ArrowBuilder(self.scope.enter_scope())

    def call(self, fun: Expression, args: List[Expression]) -> Call:
        return Call(fun, args)

    def binary_operator(self, fun: Expression, lhs: Expression, rhs: Expression) -> BinaryOperator:
        return BinaryOperator(fun, lhs, rhs)

    def unary_operator(self, fun: Expression, expr: Expression) -> UnaryOperator:
        return UnaryOperator(fun, expr)


class FunctionBuilder(AstBuilder):
    def __init__(self, context):
        super().__init__(context)
        self.fun = Function()

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
    def __init__(self, context):
        super().__init__(context)


class MatchBuilder(AstBuilder):
    def __init__(self, context):
        super().__init__(context)


class ArrowBuilder(AstBuilder):
    def __init__(self, context):
        super().__init__(context)
        self.args = []
        self.return_type = None

    def make(self):
        return Arrow(self.args, self.return_type)


def builder_test():
    global_scope = Scope()
    builder = AstBuilder(global_scope)

    b = Builtin('Type', None)

    builder.bind('Type', b)
    builder.builtin('Float', builder.reference('Type'))
    builder.bind('a', builder.value(2, builder.reference('Float')))

    fun_builder = builder.function()\
        .args([('a', builder.reference('Float')), ('b', builder.reference('Float'))])\
        .return_type(builder.reference('Float'))

    body = fun_builder.binary_operator(
        fun_builder.reference('+'),
        fun_builder.reference('a'),
        fun_builder.reference('b')
    )

    fun_builder.body(body)
    builder.bind('add', fun_builder.make())

    global_scope.dump()
    """
    Scope
    ------------------------------------------------
    0 | Type  | Builtin(name='Type', type=None)
    1 | Float | Builtin(name='Float', type=VariableRef(index=0))
    2 | a     | Value(value=2, type=VariableRef(index=1))
    3 | add   | Function(
        args=[Variable(name='a', type=VariableRef(index=1)), Variable(name='b', type=VariableRef(index=1))],
        return_type=VariableRef(index=1),
        body=BinaryOperator(function=VariableRef(index=-1), args=[VariableRef(index=3), VariableRef(index=4)]),
        is_lambda=False)
    """


if __name__ == '__main__':
    builder_test()
