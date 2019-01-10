from kiwi.type.trace import *


def main():
    import sys
    sys.stderr = sys.stdout

    from kiwi.print import to_string
    from kiwi.builder import AstBuilder
    from kiwi.builtin import make_scope

    ctx = make_scope()
    builder = AstBuilder(ctx)
    float_type = builder.reference('Float')

    # Make the Add Function
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
    # Done

    # Make a Call to `add`
    add_fun = builder.reference('add')
    two = builder.call(add_fun, [builder.value(1, float_type), builder.value(2, float_type)])
    # Done

    # ctx.dump()

    print('-' * 80)
    no_types = to_string(fun, ctx)

    print('-' * 80)
    result = type_trace(two, ctx)
    #result = type_trace(fun, ctx)

    print('-' * 80)
    fun_str = to_string(fun, ctx)
    print('-' * 80)
    print('   User Input: ', no_types)
    print('Deduced Types: ', fun_str)
    print('       Result: ', result[1])


if __name__ == '__main__':
    main()
