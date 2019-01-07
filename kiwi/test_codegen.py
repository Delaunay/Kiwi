from kiwi.codegen.ir import LLVMCodeGen
from kiwi.type.trace import type_trace
from llvmlite import ir


if __name__ == '__main__':
    import sys
    sys.stderr = sys.stdout

    from kiwi.print import to_string
    from kiwi.builder import AstBuilder
    from kiwi.builtin import make_scope
    from kiwi.type.deduce import type_deduce
    from kiwi.test_user_types import function_test_add_def

    ctx = make_scope()
    builder = AstBuilder(ctx)

    float_type = builder.reference('Float')
    fun = function_test_add_def(builder)
    two = builder.call(fun, [builder.value(1, float_type), builder.value(2, float_type)])

    original = to_string(fun)

    print(type_trace(two, ctx))

    new = to_string(fun)
    ftype = to_string(fun.type)

    print('-' * 80)
    print(original)
    print(new)
    print('Fun Type: {}'.format(ftype))
    print('-' * 80)

    v = LLVMCodeGen(ir.Module(name='test'))

    r = v.visit(fun)

    print(v.module)

