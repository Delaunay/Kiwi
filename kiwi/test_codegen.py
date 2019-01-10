from kiwi.codegen.ir import llvm_codegen
from kiwi.type.trace import type_trace
from llvmlite import ir


def main():
    import sys
    sys.stderr = sys.stdout

    from kiwi.print import to_string
    from kiwi.builder import AstBuilder
    from kiwi.builtin import make_scope
    from kiwi.test_add import function_test_add_def

    ctx = make_scope()
    builder = AstBuilder(ctx)

    float_type = builder.reference('Float')

    bind = function_test_add_def(builder, return_bind=True)

    two = builder.call(bind.expr, [builder.value(1, float_type), builder.value(2, float_type)])

    original = to_string(bind)

    print('-' * 80)
    type_trace(two, ctx)
    print('-' * 80)
    new = to_string(bind)
    print('-' * 80)
    ftype = to_string(bind.expr.type)

    print('-' * 80)
    print(original)
    print('\nAfter Inference:')
    print('-----------------')
    print(new)
    print()
    print('Fun Type: {}'.format(ftype))
    print('-' * 80)

    module = ir.Module(name='test')
    r = llvm_codegen(module, ctx, bind)

    print('-' * 80)
    print(r)
    print('-' * 80)
    print(module)
    print('-' * 80)


if __name__ == '__main__':
    main()
