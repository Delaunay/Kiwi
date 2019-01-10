from kiwi.expressions import *
from kiwi.builder import AstBuilder
from kiwi.builtin import make_scope
from kiwi.interpreter import keval


# -----------------------------------------------
def function_test_add_def(builder: AstBuilder, return_bind=False):
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
    bind = builder.bind('add', fun)

    if return_bind:
        return bind

    return fun


def function_test_add_call(builder: AstBuilder, a: int, b: int) -> Call:
    float_type = builder.reference('Float')
    add_fun = builder.reference('add')
    return builder.call(add_fun, [builder.value(a, float_type), builder.value(b, float_type)])


def function_test_add(builder, ctx):
    function_test_add_def(builder)

    for i in range(0, 10):
        for j in range(0, 10):
            call = function_test_add_call(builder, i, j)

            result = keval(call, ctx)
            print('-' * 80)

            if result.value != i + j:
                raise RuntimeError('{} + {} != {}'.format(i, j, result.value))
# -----------------------------------------------


def main():
    import sys
    sys.stderr = sys.stdout

    ctx = make_scope()
    builder = AstBuilder(ctx)

    function_test_add(builder, ctx)


if __name__ == '__main__':
    main()
