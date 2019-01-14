from kiwi.interpreter import keval
from kiwi.type.trace import *
from kiwi.print import to_string
from kiwi.builder import AstBuilder
from kiwi.builtin import make_scope
from kiwi.test_user_types import make_struct_constructor


def is_error(should_work, error):
    if should_work:
        raise error
    else:
        pass


def return_val(builder, expr):
    return builder.unary_operator(builder.reference('return'), expr)


def make_branching_call(builder: AstBuilder):
    ftype = builder.reference('Float')

    # Make the Add Function
    fun = builder.function()
    fun.args([('pred', ftype), ('x', None), ('y', None)])
    fun.return_type = None

    def return_val(builder, expr):
        return builder.unary_operator(builder.reference('return'), expr)

    match = fun.match()
    match.target(match.reference('pred'))
    match.add_expr_branch(match.value(0, ftype), return_val(match, match.reference('x')))
    match.add_expr_branch(match.value(1, ftype), return_val(match, match.reference('y')))

    fun.body(match.make())
    fun = fun.make()

    fun = builder.bind('branch_xy', fun)

    print()
    v_str = to_string(fun)
    print(v_str)
    return fun


def make_var(builder, name, type_name) -> Expression:
    variable_fun = builder.reference('variable')
    symbol_type = builder.reference('Symbol')

    return builder.call(variable_fun, [
        builder.value(name, symbol_type),
        builder.reference(type_name)
    ])


def make_vec2(builder: AstBuilder):
    struct_fun = builder.reference('struct')
    return builder.call(struct_fun, [
        make_var(builder, 'x', 'Float'),
        make_var(builder, 'y', 'Float')])


def make_vec2_ctor(builder: AstBuilder):
    struct_call = make_vec2(builder)
    struct_str = to_string(struct_call)
    print('-' * 80)
    struct_constructor = keval(struct_call, builder.scope)
    print('-' * 80)
    result_str = to_string(struct_constructor)

    print('\n IN > {} \nOUT > {}'.format(struct_str, result_str))
    return struct_constructor


def make_unpacking_call(builder: AstBuilder):
    ftype = builder.reference('Float')

    vec2 = make_vec2_ctor(builder)
    builder.bind('Vec2f', vec2)

    # Make the Add Function
    fun = builder.function()
    fun.args([('vec', builder.reference('Vec2f'))])
    fun.return_type = ftype

    def return_val(builder, expr):
        return builder.unary_operator(builder.reference('return'), expr)

    match = fun.match()
    match.target(match.reference('vec'))
    match.add_unpacking(
        match.reference('Vec2f'), ['a', 'b'],
        return_val(match, match.binary_operator(
            match.reference('+'),
            match.reference('a'),
            match.reference('b')
        ))
    )

    fun.body(match.make())
    fun = fun.make()

    fun = builder.bind('unpack_xy', fun)

    print()
    v_str = to_string(fun)
    print(v_str)
    return fun


def make_type_dispatch_call(builder: AstBuilder):
    ftype = builder.reference('Float')
    itype = builder.reference('Int')

    from kiwi.test_user_types import make_union_constructor

    uctor = make_union_constructor(builder)
    builder.bind('FloatOrInt', uctor)

    # Make the Add Function
    fun = builder.function()
    fun.args([('var', builder.reference('FloatOrInt')), ('float', ftype), ('int', itype)])
    fun.return_type = ftype

    ctor = builder.reference('FloatOrInt')

    def make_float_or_int(type, value):
        return builder.call(ctor, [builder.named_arg(type, value)])

    def return_val(builder, expr):
        return builder.unary_operator(builder.reference('return'), expr)

    match = fun.match()
    match.target(match.reference('var'))
    match.add_unpacking(
        'float', ['a'],
        return_val(match, make_float_or_int('float', match.binary_operator(
            match.reference('+'),
            match.reference('a'),
            match.reference('float')
        )))
    )


    match.add_unpacking(
        'int', ['a'],
        return_val(match, make_float_or_int('int', match.binary_operator(
            match.reference('int_add'),
            match.reference('a'),
            match.reference('int')
        )))
    )

    fun.body(match.make())
    fun = fun.make()

    fun = builder.bind('dispatch_xy', fun)

    print()
    v_str = to_string(fun)
    print(v_str)
    return fun


def test_branching(builder: AstBuilder, ctx):
    ftype = builder.reference('Float')
    cases = [
        (1, 2, 3, True, 3),
        (0, 2, 3, True, 2),
        (1, 5, 6, True, 6),
        (0, 5, 6, True, 5),
        (2, 2, 3, False, None),
    ]

    for (a1, a2, a3, should_work, result) in cases:
        try:
            call = builder.call(builder.reference('branch_xy'), [
                builder.value(a1, ftype),
                builder.value(a2, ftype),
                builder.value(a3, ftype)])

            type_trace(call, ctx)
            str_call = to_string(call, ctx)
            print()
            v = keval(call, ctx)
            print()
            v_str = to_string(v)
            print()

            print(' IN > {}'.format(str_call))
            print('OUT > {}'.format(v_str))
            print('-' * 80)

            assert v.value == result

            if not should_work:
                print('error')

        except Exception as e:
            is_error(should_work, e)


def test_unpacking(builder: AstBuilder, ctx):
    ftype = builder.reference('Float')
    cases = [
        (1, 5, 6),
        (2, 4, 6),
        (3, 3, 6),
        (4, 2, 6),
        (5, 1, 6),
        (10, 10, 20)
    ]

    ctor = builder.reference('Vec2f')

    for (a1, a2, result) in cases:
        call = builder.call(
            builder.reference('unpack_xy'), [
                builder.call(ctor, [builder.value(a1, ftype), builder.value(a2, ftype)])
            ]
        )

        type_trace(call, ctx)
        str_call = to_string(call, ctx)
        print()
        v = keval(call, ctx)
        print()
        v_str = to_string(v)
        print()

        print(' IN > {}'.format(str_call))
        print('OUT > {}'.format(v_str))
        print('-' * 80)

        assert v.value == result


def test_dispatch(builder: AstBuilder, ctx):
    from kiwi.operations.equality import kequal

    ftype = builder.reference('Float')
    itype = builder.reference('Int')
    cases = [
        ('float', 1, 'Float', 1.0, 2, 2),
        # ('int'  , 1, 'Int'  , 1.0, 2, 3),
    ]

    ctor = builder.reference('FloatOrInt')

    for (name, a1, a1type, a2, a3, result) in cases:
        call = builder.call(
            builder.reference('dispatch_xy'), [
                builder.call(ctor, [builder.named_arg(name, builder.value(a1, builder.reference(a1type)))]),
                builder.value(a2, ftype),
                builder.value(a3, itype)
            ],
        )

        type_trace(call, ctx)
        str_call = to_string(call, ctx)
        print()
        v = keval(call, ctx)
        print()
        v_str = to_string(v)
        print()

        result = keval(builder.call(ctor, [builder.named_arg(name, builder.value(result, builder.reference(a1type)))]),
                       ctx)

        print()
        print(' IN > {}'.format(str_call))
        print('OUT > {} == {}'.format(v_str, to_string(result, ctx)))

        print()
        assert kequal(v, result, ctx)
        print('-' * 80)


def main():
    import sys
    sys.stderr = sys.stdout

    ctx = make_scope()
    builder = AstBuilder(ctx)
    float_type = builder.reference('Float')

    make_branching_call(builder)
    test_branching(builder, ctx)
    print()

    make_unpacking_call(builder)
    test_unpacking(builder, ctx)
    print()

    make_type_dispatch_call(builder)
    test_dispatch(builder, ctx)


if __name__ == '__main__':
    main()
