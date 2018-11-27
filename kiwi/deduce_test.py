from kiwi.expressions import *
from kiwi.builder import AstBuilder
from kiwi.builtin import make_scope
from kiwi.interpreter import keval


# -----------------------------------------------
def function_test_add_def(builder: AstBuilder):
    float_type = builder.reference('Float')

    fun = builder.function()
    fun.args([('x', None), ('y', None)])
    fun.return_type = None

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


def test():
    import sys
    sys.stderr = sys.stdout
    from kiwi.print import to_string
    from kiwi.type.deduce import type_deduce

    ctx = make_scope()
    builder = AstBuilder(ctx)
    fun = function_test_add_def(builder)

    type_deduce(fun, ctx)

    print(to_string(fun, ctx))
    print(to_string(fun, ctx))
    print(to_string(fun.type, ctx))


if __name__ == '__main__':
    test()
