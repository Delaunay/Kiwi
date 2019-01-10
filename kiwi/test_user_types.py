from kiwi.expressions import *
from kiwi.builder import AstBuilder
from kiwi.builtin import make_scope
from kiwi.interpreter import keval
from kiwi.type.trace import type_trace
from kiwi.codegen.ir import llvm_codegen

from kiwi.print import to_string
from llvmlite import ir

ctx = make_scope()
builder = AstBuilder(ctx)

ftype = builder.reference('Float')
itype = builder.reference('Int')


def make_var(builder, name, type_name) -> Expression:
    variable_fun = builder.reference('variable')
    symbol_type = builder.reference('Symbol')

    return builder.call(variable_fun, [
        builder.value(name, symbol_type),
        builder.reference(type_name)
    ])


def union_test_make(builder: AstBuilder):
    union_fun = builder.reference('union')
    return builder.call(union_fun, [
        make_var(builder, 'float', 'Float'),
        make_var(builder, 'int', 'Int')])


def struct_test_make(builder: AstBuilder):
    struct_fun = builder.reference('struct')
    return builder.call(struct_fun, [
        make_var(builder, 'float', 'Float'),
        make_var(builder, 'int', 'Int')])


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


def show_result(expr, ctx):
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

            type_trace(union_obj, ctx)
            show_result(union_obj, ctx)

            bind = builder.bind('val', union_obj)

            module = ir.Module(name='test')

            # -- dummy main function
            fun = ir.Function(module, ftype=ir.FunctionType(ir.VoidType(), []), name='main')
            block = fun.append_basic_block('main_block')
            irbuilder = ir.IRBuilder(block)
            # ----

            llvm_union = llvm_codegen(module=module, scope=ctx, expr=bind, builder=irbuilder)
            print()
            print(llvm_union.expr)
            print()

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

            type_trace(struct_obj, ctx)
            show_result(struct_obj, ctx)

            bind = builder.bind('val', struct_obj)

            module = ir.Module(name='test')

            # -- dummy main function
            fun = ir.Function(module, ftype=ir.FunctionType(ir.VoidType(), []), name='main')
            block = fun.append_basic_block('main_block')
            irbuilder = ir.IRBuilder(block)
            # ----

            llvm_struct = llvm_codegen(module=module, scope=ctx, expr=bind, builder=irbuilder)
            print()
            print(llvm_struct.expr)
            print()

        except Exception as e:
            is_error(should_work, e)


def main():
    import sys
    sys.stderr = sys.stdout

    uctor = make_union_constructor(builder)
    print()
    sctor = make_struct_constructor(builder)
    print()

    builder.bind('FloatAndInt', sctor)
    builder.bind('FloatOrInt', uctor)

    test_struct(builder)
    test_union(builder)


if __name__ == '__main__':
    main()
