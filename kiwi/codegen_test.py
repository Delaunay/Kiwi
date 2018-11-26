from kiwi.codegen.ir import LLVMCodeGen
from llvmlite import ir


if __name__ == '__main__':
    import sys
    sys.stderr = sys.stdout

    from kiwi.print import to_string
    from kiwi.builder import AstBuilder
    from kiwi.builtin import make_scope
    from kiwi.type.deduce import type_deduce
    from kiwi.user_types_test import function_test_add_def

    ctx = make_scope()
    builder = AstBuilder(ctx)
    fun = function_test_add_def(builder)

    v = LLVMCodeGen(ir.Module(name='test'))

    print(fun)
    print(type_deduce(fun, ctx))

    r = v.visit(fun)

    print(v.module)

