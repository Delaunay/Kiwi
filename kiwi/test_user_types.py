from kiwi.expressions import *
from kiwi.builder import AstBuilder
from kiwi.builtin import make_scope
from kiwi.interpreter import keval


# -----------------------------------------------
def function_test_add_def(builder: AstBuilder):
    float_type = builder.reference('Float')

    fun = builder.function()
    fun.args([('x', float_type), ('y', float_type)])
    fun.return_type = float_type

    body = fun.unary_operator(
        fun.reference('return'),
        fun.binary_operator(
            fun.reference('+'),
            fun.reference('x'),
            fun.reference('y')
        )
    )

    fun.body(body)
    fun = fun.make()
    builder.bind('add', fun)
    return fun


def function_test_add_call(builder: AstBuilder, a: int, b: int) -> Call:
    float_type = builder.reference('Float')
    add_fun = builder.reference('add')
    return builder.call(add_fun, [builder.value(a, float_type), builder.value(b, float_type)])


def function_test_add(builder):
    function_test_add_def(builder)

    for i in range(0, 10):
        for j in range(0, 10):
            call = function_test_add_call(builder, i, j)
            result = keval(call, ctx)

            if result.value != i + j:
                raise RuntimeError('{} + {} != {}'.format(i, j, result.value))
# -----------------------------------------------


def make_var(name, type_name) -> Expression:
    variable_fun = builder.reference('variable')
    symbol_type = builder.reference('Symbol')

    return builder.call(variable_fun, [
        builder.value(name, symbol_type),
        builder.reference(type_name)
    ])


def union_test_make(builder: AstBuilder):
    union_fun = builder.reference('union')
    return builder.call(union_fun, [make_var('a', 'Float'), make_var('b', 'Int')])


def struct_test_make(builder: AstBuilder):
    struct_fun = builder.reference('struct')
    return builder.call(struct_fun, [make_var('a', 'Float'), make_var('b', 'Int')])


if __name__ == '__main__':
    import sys
    sys.stderr = sys.stdout
    from kiwi.print import to_string

    ctx = make_scope()
    builder = AstBuilder(ctx)
    function_test_add(builder)

    arg = make_var('a', 'Float')
    str = to_string(arg)
    print(str)
    print(to_string(keval(arg, ctx)))

    print('-' * 80)

    call = struct_test_make(builder)
    call_str = to_string(call)
    result_str = to_string(keval(call, ctx))

    print('\n IN > {} \nOUT > {}'.format(call_str, result_str))

