from kiwi.interpreter import keval
from kiwi.type.trace import *


if __name__ == '__main__':
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
    fun.args([('pred', None), ('x', None), ('y', None)])
    fun.return_type = None

    def return_val(builder, expr):
        return builder.unary_operator(builder.reference('return'), expr)

    match = fun.match()
    match.target(match.reference('pred'))
    match.add_expr_branch(match.value(0, match.reference('Float')), return_val(match, match.reference('x')))
    match.add_expr_branch(match.value(1, match.reference('Float')), return_val(match, match.reference('y')))

    fun.body(match.make())
    fun = fun.make()

    fun = builder.bind('select_xy', fun)

    str_match = to_string(fun, ctx)

    print('-' * 80)
    print(str_match)
    print('-' * 80)

    ftype = match.reference('Float')

    call1 = builder.call(builder.reference('select_xy'), [
        builder.value(0, ftype),
        builder.value(2, ftype),
        builder.value(3, ftype)])

    call2 = builder.call(builder.reference('select_xy'), [
        builder.value(1, ftype),
        builder.value(2, ftype),
        builder.value(3, ftype)])

    str_call = to_string(call1, ctx)

    print('-' * 80)
    print(str_call)
    print('-' * 80)

    v1 = keval(call1, ctx)
    print('-' * 80)
    v2 = keval(call2, ctx)

    print('-' * 80)
    print(v1)

    assert v1.value == 2
    assert v2.value == 3
