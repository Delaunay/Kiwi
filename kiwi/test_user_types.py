from kiwi.expressions import *
from kiwi.builder import AstBuilder
from kiwi.builtin import make_scope
from kiwi.interpreter import keval
from kiwi.type.trace import type_trace


def make_var(name, type_name) -> Expression:
    variable_fun = builder.reference('variable')
    symbol_type = builder.reference('Symbol')

    return builder.call(variable_fun, [
        builder.value(name, symbol_type),
        builder.reference(type_name)
    ])


def union_test_make(builder: AstBuilder):
    union_fun = builder.reference('union')
    return builder.call(union_fun, [make_var('float', 'Float'), make_var('int', 'Int')])


def struct_test_make(builder: AstBuilder):
    struct_fun = builder.reference('struct')
    return builder.call(struct_fun, [make_var('float', 'Float'), make_var('int', 'Int')])


def make_struct_constructor(builder: AstBuilder):
    struct_call = struct_test_make(builder)
    struct_str = to_string(struct_call)
    print('-' * 80)
    struct_constructor = keval(struct_call, ctx)
    print('-' * 80)
    result_str = to_string(struct_constructor)

    print('\n IN > {} \nOUT > {}'.format(struct_str, result_str))
    return struct_constructor


def make_union_constructor(builder: AstBuilder):
    union_call = union_test_make(builder)
    union_str = to_string(union_call)
    print('-' * 80)
    union_constructor = keval(union_call, ctx)
    print('-' * 80)
    result_str = to_string(union_constructor)

    print('\n IN > {} \nOUT > {}'.format(union_str, result_str))
    return union_constructor


def is_error(should_work, error):
    if should_work:
        raise error
    else:
        pass


def show_result(expr):
    type_trace(expr, ctx)
    print()
    val = keval(expr, ctx)
    print()
    val_str = to_string(val, ctx)
    print()
    print(val_str)
    print('-' * 80)


def test_union(builder: AstBuilder):
    cases = [
        ('float', itype, False),
        ('float', ftype, True),
        ('int'  , itype, True),
        ('int'  , ftype, False),
        ('a', ftype, False),
        ('b', itype, False),
    ]

    for (name, type, should_work) in cases:
        try:
            union_obj = builder.call(
                builder.reference('FloatOrInt'),
                [builder.named_arg(name, builder.value(3, type))]
            )

            show_result(union_obj)

        except Exception as e:
            is_error(should_work, e)


def test_struct(builder: AstBuilder):
    cases = [
        (itype, itype, False),
        (ftype, ftype, False),
        (itype, ftype, False),
        (ftype, itype, True)
    ]

    for (t1, t2, should_work) in cases:
        try:
            struct_obj = builder.call(
                builder.reference('FloatAndInt'),
                [builder.value(2, t1), builder.value(3, t2)]
            )

            show_result(struct_obj)

        except Exception as e:
            is_error(should_work, e)


if __name__ == '__main__':
    import sys
    sys.stderr = sys.stdout
    from kiwi.print import to_string

    ctx = make_scope()
    builder = AstBuilder(ctx)

    ftype = builder.reference('Float')
    itype = builder.reference('Int')

    uctor = make_union_constructor(builder)
    print()
    sctor = make_struct_constructor(builder)
    print()

    builder.bind('FloatAndInt', sctor)
    builder.bind('FloatOrInt', uctor)

    struct_obj = builder.call(
        builder.reference('FloatAndInt'),
        [builder.value(2, ftype), builder.value(3, itype)]
    )

    test_struct(builder)
    test_union(builder)
