from typing import *

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.builder import AstBuilder


def _make_default_scope() -> Scope:
    """
        Make scope with default builtins the top level scope will always be the builtin scope
        The Builtin scope should be identical to all scopes
    """

    ctx = Scope()
    builder = AstBuilder(ctx)
    builder.bind('Type',  Builtin('Type',  None))

    def ref(name: str) -> Expression:
        return builder.reference(name)

    # Default high level types
    builder.bind('Float', Builtin('Float', ref('Type')))
    builder.bind('Int',   Builtin('Int',   ref('Type')))
    builder.bind('Bool',  Builtin('Bool',  ref('Type')))

    # return T -> T
    builder.builtin('return', Arrow([ref('Type')], ref('Type')))
    builder.builtin('yield',  Arrow([ref('Type')], ref('Type')))

    # builtin functions
    builder.bind('+', Builtin('+', Arrow([ref('Float'), ref('Float')], ref('Float'))))
    builder.bind('*', Builtin('*', Arrow([ref('Float'), ref('Float')], ref('Float'))))
    builder.bind('/', Builtin('/', Arrow([ref('Float'), ref('Float')], ref('Float'))))
    builder.bind('-', Builtin('-', Arrow([ref('Float'), ref('Float')], ref('Float'))))

    # ATM it will not work because the scope is a dum dictionary
    builder.bind('+', Builtin('+', Arrow([ref('Int'), ref('Int')], ref('Int'))))
    builder.bind('*', Builtin('*', Arrow([ref('Int'), ref('Int')], ref('Int'))))
    builder.bind('/', Builtin('/', Arrow([ref('Int'), ref('Int')], ref('Int'))))
    builder.bind('-', Builtin('-', Arrow([ref('Int'), ref('Int')], ref('Int'))))

    return ctx


__parent_scope = _make_default_scope()


def make_scope() -> Scope():
    return __parent_scope.enter_scope()


def _builder() -> AstBuilder:
    return AstBuilder(make_scope())


type_int = _builder().reference('Int')
type_float = _builder().reference('Float')
type_bool = _builder().reference('Bool')


def make_int(args: List[Value]) -> Value:
    return Value(args[0].value, type_int)


def make_float(args: List[Value]) -> Value:
    return Value(args[0].value, type_float)


def add_float(args: List[Value]) -> Value:
    return Value(args[0].value + args[1].value, type_float)


def add_int(args: List[Value]) -> Value:
    return Value(args[0].value + args[1].value, type_int)


def mult_float(args: List[Value]) -> Value:
    return Value(args[0].value + args[1].value, type_float)


def mult_int(args: List[Value]) -> Value:
    return Value(args[0].value + args[1].value, type_int)


def div_float(args: List[Value]) -> Value:
    return Value(args[0].value / args[1].value, type_float)


def div_int(args: List[Value]) -> Value:
    return Value(args[0].value / args[1].value, type_int)


def sub_float(args: List[Value]) -> Value:
    return Value(args[0].value - args[1].value, type_float)


def sub_int(args: List[Value]) -> Value:
    return Value(args[0].value - args[1].value, type_int)


def return_fun(args: List[Expression]) -> Expression:
    return args[0]


# Table for the evaluator
builtins = {
    '+': (2, add_int),
    '*': (2, mult_int),
    '/': (2, div_int),
    '-': (2, sub_int),
    'return': (1, return_fun),
    'Int': (1, make_int)
}


def builtins_implementation():
    return builtins
