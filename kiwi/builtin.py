from typing import *

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.builder import AstBuilder


def _make_default_scope() -> Scope:
    """
        Make scope with default builtins the top level scope will always be the builtin scope
        The Builtin scope should be identical to all scopes
    """

    ctx = Scope(name='root')
    builder = AstBuilder(ctx)
    builder.bind('Type',  Builtin('Type',  None))

    def ref(name: str) -> Expression:
        return builder.reference(name)

    # Default high level types
    builder.bind('Expression', Builtin('Expression', ref('Type')))
    builder.bind('Symbol', Builtin('Symbol', ref('Type')))
    # Symbol, Type Pair
    builder.bind('Variable', Builtin('Variable', ref('Type')))

    builder.bind('Float', Builtin('Float', ref('Type')))
    builder.bind('Int',   Builtin('Int',   ref('Type')))
    builder.bind('Bool', Builtin('Bool', ref('Type')))

    def make_var(name, type):
        return builder.variable(name, ref(type))

    # return T -> T
    builder.builtin('return', Arrow([make_var('T', 'Type')], Arrow([make_var('r', 'T')], ref('T'))))
    builder.builtin('yield', Arrow([make_var('T', 'Type')], Arrow([make_var('r', 'T')], ref('T'))))

    # Compile Time Function that makes up the language
    # We are going to need more like match/value/....
    # one function per AST node
    # The type should be (List[var] -> Type)
    # Where Var = struct(name: str, type: Type)

    builder.bind('variable', Builtin('variable',
                                     Arrow([make_var('name', 'Symbol'), make_var('type', 'Expression')], ref('Variable'))))

    builder.builtin('union',  Builtin('union',  Arrow([make_var('args', 'Variable')], ref('Type'))))
    builder.builtin('struct', Builtin('struct', Arrow([make_var('args', 'Variable')], ref('Type'))))
    builder.builtin('lambda', Builtin('lambda', Arrow([make_var('args', 'Variable')], ref('Type'))))
    #

    # builtin functions
    builder.bind('+', Builtin('+', Arrow([make_var('a', 'Float'), make_var('b', 'Float')], ref('Float'))))
    builder.bind('*', Builtin('*', Arrow([make_var('a', 'Float'), make_var('b', 'Float')], ref('Float'))))
    builder.bind('/', Builtin('/', Arrow([make_var('a', 'Float'), make_var('b', 'Float')], ref('Float'))))
    builder.bind('-', Builtin('-', Arrow([make_var('a', 'Float'), make_var('b', 'Float')], ref('Float'))))

    # ATM it will not work because the scope is a dumb dictionary
    builder.bind('+', Builtin('+', Arrow([make_var('a', 'Int'), make_var('b', 'Int')], ref('Int'))))
    builder.bind('*', Builtin('*', Arrow([make_var('a', 'Int'), make_var('b', 'Int')], ref('Int'))))
    builder.bind('/', Builtin('/', Arrow([make_var('a', 'Int'), make_var('b', 'Int')], ref('Int'))))
    builder.bind('-', Builtin('-', Arrow([make_var('a', 'Int'), make_var('b', 'Int')], ref('Int'))))

    return ctx


__parent_scope = _make_default_scope()


def make_scope(name='module_scope') -> Scope():
    return __parent_scope.enter_scope(name)


def _builder() -> AstBuilder:
    return AstBuilder(make_scope())


type_int = _builder().reference('Int')
type_float = _builder().reference('Float')
type_bool = _builder().reference('Bool')
type_variable = _builder().reference('Variable')
type_symbol = _builder().reference('Symbol')


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


def make_variable(args: List[Expression]) -> Expression:
    if len(args) != 2:
        raise RuntimeError('`Variable` constructor expects 2 arguments `(name: type)`')

    if not isinstance(args[0], Value): # and isinstance(args[0].type, ):
        raise RuntimeError('`Variable` constructor expects the fist argument to be a Symbol')
    else:
        type = args[0].type
        if type.name != 'Symbol':
            raise RuntimeError('`Variable` constructor expects the fist argument to be a Symbol')

    return Value(Variable(name=args[0].value, type=args[1]), type=type_variable)


def make_union(args: List[Expression]) -> Expression:
    # arg is a value of Variables
    for arg in args:
        if not isinstance(arg, Value):
            raise RuntimeError('`union` constructor expects a list of variables `Variable(name, type)`')

    return Union([arg.value for arg in args])


def make_struct(args: List[Expression]) -> Expression:
    for arg in args:
        if not isinstance(arg, Value):
            raise RuntimeError('`struct` constructor expects a list of variables `Variable(name, type)`')

    return Struct([arg.value for arg in args])


def make_lambda(args: List[Expression]) -> Expression:
    body = args[-1]
    args = args[:-1]

    for arg in args:
        if not isinstance(arg, Value):
            raise RuntimeError('`lambda` constructor expects a list of variables `Variable(name, type)`')

    return Function(args=[arg.value for arg in args], body=body)


n = None

# Table for the evaluator
builtins = {
    '+': (2, add_int),
    '*': (2, mult_int),
    '/': (2, div_int),
    '-': (2, sub_int),
    'return': (1, return_fun),
    'Int': (1, make_int),
    'variable': (2, make_variable),
    'lambda': (n, make_lambda),
    'struct': (n, make_struct),
    'union': (n, make_union)
}


def builtins_implementation():
    return builtins
