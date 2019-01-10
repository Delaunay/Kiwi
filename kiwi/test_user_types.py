from kiwi.expressions import *
from kiwi.builder import AstBuilder
from kiwi.builtin import make_scope
from kiwi.interpreter import keval


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


if __name__ == '__main__':
    import sys
    sys.stderr = sys.stdout
    from kiwi.print import to_string

    ctx = make_scope()
    builder = AstBuilder(ctx)

    ftype = builder.reference('Float')

    uctor = make_union_constructor(builder)
    print()
    sctor = make_struct_constructor(builder)

    builder.bind('sctor', sctor)
    builder.bind('FloatOrInt', uctor)

    struct_obj = builder.call(
        builder.reference('sctor'),
        [builder.value(2, ftype), builder.value(3, ftype)]
    )

    union_obj = builder.call(
        builder.reference('FloatOrInt'),
        [builder.named_arg('float', builder.value(3, ftype))]
    )

    print()
    val = keval(struct_obj, ctx)
    print('-' * 80)
    val_str = to_string(val, ctx)

    print()
    print(val_str)

    print()
    val = keval(union_obj, ctx)
    print('-' * 80)
    val_str = to_string(val, ctx)

    print()
    print(val_str)

