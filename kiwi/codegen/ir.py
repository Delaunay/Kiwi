from functools import partial
from functools import reduce

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.debug import trace

from llvmlite import ir

debug_mode = True
trace = partial(trace, mode=debug_mode, name='[llvm_codegen] ')



class LLVMCodeGen(Visitor):
    def __init__(self, module: ir.Module):
        super().__init__()
        self.module = module
        self.parents = []

    def visit(self, a: Expression, depth=0) -> Any:
        if a is not None:
            return a.visit(self, depth)

    # Typed Expression
    def variable(self, var: Variable, depth=0) -> Any:
        return ir.NamedValue(parent=None, name=var.name, type=self.visit(var.type, depth + 1))

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        raise NotImplementedError

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')
        return ir.Constant(constant=val.value, typ=self.visit(val.type, depth + 1))

    def struct(self, struct: Struct, depth=0) -> Any:
        return ir.LiteralStructType(elems=[self.visit(m, depth + 1) for m in struct.members])

    def union(self, union: Union, depth=0) -> Any:
        return ir.LiteralStructType(elems=[self.visit(m, depth + 1) for m in union.members])

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        trace(depth, 'arrow')
        return ir.FunctionType(
            self.visit(arrow.return_type, depth + 1),
            [self.visit(arg, depth + 1) for arg in arrow.args])

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference')
        return get(ref.name)

    # Those should be resolved at eval
    def bind(self, bind: Bind, depth=0) -> Any:
        raise NotImplementedError

    def function(self, fun: Function, depth=0) -> Any:
        fun = ir.Function(
            self.module,
            ftype=None,
            name=None
        )

        block = fun.append_basic_block(name='fun_block')
        builder = ir.IRBuilder(block)

        return fun

    def block(self, block: Block, depth=0) -> Any:
        return ir.Block()

    def match(self, match: Match, depth=0) -> Any:
        raise NotImplementedError

    def conditional(self, cond: Conditional, depth=0) -> Any:
        raise NotImplementedError

    def call(self, call: Call, depth=0) -> Any:
        return ir.CallInstr()

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        return ir.CallInstr()

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        return ir.CallInstr()


if __name__ == '__main__':
    import sys
    sys.stderr = sys.stdout

    from kiwi.print import to_string
    from kiwi.builder import AstBuilder
    from kiwi.builtin import make_scope

    ctx = make_scope()
    builder = AstBuilder(ctx)

    int_t = builder.reference('Int')
    float_t = builder.reference('Float')

    print(type_equal(builder.value(2, int_t), builder.value(2, float_t), ctx))
    print(type_equal(builder.value(2, float_t), builder.value(2, float_t), ctx))
    arw = builder.arrow()
    arw.args([int_t, int_t])
    arw.return_type(int_t)
    a1 = arw.make()

    arw = builder.arrow()
    arw.args([int_t, int_t])
    arw.return_type(int_t)
    a2 = arw.make()

    arw = builder.arrow()
    arw.args([int_t, int_t])
    arw.return_type(float_t)
    a3 = arw.make()

    print(type_equal(a1, a2, ctx))
    print(type_equal(a1, a3, ctx))
