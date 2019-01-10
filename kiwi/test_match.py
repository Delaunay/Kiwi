from kiwi.interpreter import keval
from kiwi.type.trace import *
from kiwi.print import to_string
from kiwi.builder import AstBuilder
from kiwi.builtin import make_scope


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


def main():
    import sys
    sys.stderr = sys.stdout

    ctx = make_scope()
    builder = AstBuilder(ctx)
    float_type = builder.reference('Float')

    make_branching_call(builder)

    test_branching(builder, ctx)


if __name__ == '__main__':
    main()
