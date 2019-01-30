import functools

from kiwi.expressions import *
from kiwi.environment import Scope
from kiwi.builder import AstBuilder
from kiwi.debug import trace

debug_mode = False
trace = functools.partial(trace, mode=debug_mode, name='[to_string] ')


class ToStringV(Visitor):
    def __init__(self, ctx=Scope()):
        super(ToStringV).__init__()
        # enter a new scope to not affect the original scope
        self.env_stack: Scope = ctx.enter_scope(name='visitor_scope')
        self.bind_name = []
        self._i = 0
        self.line = 0
        self.indent_size = 2
        self.indent_char = ' '

    def indent(self):
        self._i += self.indent_size
        return self._i

    def deindent(self):
        self._i = max(0, self._i - self.indent_size)
        return self._i

    def newline(self):
        self.line += 1
        return '\n'

    def get_indent(self):
        return self.indent_char * self.indentation

    def get_next_indent(self):
        self.indent()
        return self.get_indent()

    @property
    def indentation(self):
        return self._i

    @staticmethod
    def run(expr: Expression, ctx=Scope()):
        return ToStringV(ctx).visit(expr, depth=0)

    def resolve_reference(self, ref):
        try:
            if isinstance(ref, VariableRef):
                return self.env_stack.get_expression(ref)
        except:
            pass

        return ref

    def block(self, block: Block, depth=0) -> str:
        rep = []

        for expr in block.expressions:
            rep.append(self.visit(expr, depth + 1) + self.newline())

        return (' ' * self.indentation).join(rep)

    def value(self, val: Value, depth=0) -> Any:
        trace(depth, 'value')

        type = self.resolve_reference(val.type)

        if isinstance(val, StructValue):  # isinstance(type, Struct):
            values = map(lambda x: self.visit(x, depth + 1), val.value)
            return '{}({})'.format(self.visit(val.type, depth + 1), ', '.join(values))

        if isinstance(val, UnionValue):  # isinstance(type, Union):
            return '{}.{}({})'.format(
                self.visit(val.type, depth + 1),
                val.name,
                self.visit(val.value, depth + 1)
            )

        return '{}: {}'.format(str(val.value), self.visit(val.type, depth + 1))

    def variable(self, var: Variable, depth=0) -> Any:
        trace(depth, 'variable {}'.format(var))
        return '{}: {}'.format(str(var.name), self.visit(var.type, depth + 1))

    def reference(self, ref: VariableRef, depth=0) -> Any:
        trace(depth, 'reference {}'.format(ref))
        return ref.name
        # """
        # refs = self.visit(ref.reference, depth + 1)
        # # --------------------------------------------------------------
        # # Sanity Check
        # try:
        #     env_ref = self.env_stack.get_expression(ref, depth)
        #     # name = self.env_stack.get_name(ref)
        #     if ref.reference is not env_ref:
        #         print(ref)
        #         print('Error \n\t{} \n\t\t!=\n\t{}'.format(ref.reference, env_ref))
        # except Exception as e:
        #     print(e)
        # # --------------------------------------------------------------
        # return refs"""

    def function(self, fun: Function, depth=0) -> Any:
        trace(depth, 'function {}'.format(''))

        if len(self.bind_name) == 0:
            return self.function_lambda(fun, depth)

        return self.function_bind(self.bind_name[-1], fun, depth)

    def function_lambda(self, fun: Function, depth=0) -> Any:
        self.env_stack = self.env_stack.enter_scope(name='function_scope')

        for arg in fun.args:
            self.env_stack.insert_binding(arg.name, arg)

        args = ', '.join([self.visit(arg, depth + 1) for arg in fun.args])

        if fun.is_lambda:
            fun_str = 'lambda {}: {{ {} }}'.format(
                args, self.visit(fun.body, depth + 1))

            self.env_stack = self.env_stack.exit_scope()
            return fun_str

        # self.env_stack.dump()
        fun_str = '({}) -> {} {{ {} }}'.format(
            args,
            self.visit(fun.return_type, depth + 1),
            self.visit(fun.body, depth + 1))

        self.env_stack = self.env_stack.exit_scope()
        return fun_str

    def function_bind(self, name, fun: Function, depth=0) -> str:

        self.env_stack = self.env_stack.enter_scope(name='function_scope')

        for arg in fun.args:
            self.env_stack.insert_binding(arg.name, arg)

        args = ', '.join([self.visit(arg, depth + 1) for arg in fun.args])
        return_type = self.visit(fun.return_type, depth + 1)

        self.indent()
        fun_str = 'def {}({}) -> {}:{}{}{}'.format(
            name,
            args,
            return_type,
            self.newline(),
            self.get_indent(),
            self.visit(fun.body, depth + 1))
        self.deindent()

        return fun_str

    def match(self, match: Match, depth=0) -> Any:

        def render_pattern(pat):
            if isinstance(pat, NamePattern):
                return pat.name

            if isinstance(pat, ExpressionPattern):
                return self.visit(pat.expr, depth + 1)

            if isinstance(pat, ConstructorPattern):
                name = ''
                if isinstance(pat.name, Expression):
                    name = self.visit(pat.name, depth + 1)
                elif pat.name is not None:
                    name = pat.name

                return '{}({})'.format(name, ', '.join(list(map(lambda x: render_pattern(x), pat.args))))

        target = 'switch {}:{}'.format(self.visit(match.target, depth + 1), self.newline())
        self.indent()

        branches = []
        for pattern, result in match.branches:
            branches.append(
                '{}case {}:{}{}{}{}'.format(
                    self.get_indent(), render_pattern(pattern), self.newline(),
                    self.get_next_indent(), self.visit(result, depth + 1), self.newline())
            )
            self.deindent()

        if match.default is not None:
            branches.append(
                '{}case {}:{}{}{}{}'.format(
                     self.get_indent(), '_', self.newline(),
                     self.get_next_indent(), self.visit(match.default, depth + 1), self.newline())
            )
            self.deindent()

        self.deindent()

        return '{}{}'.format(target, ''.join(branches))

    def bind(self, bind: Bind, depth=0) -> Any:
        # FIXME nested bind
        self.bind_name.append(bind.name)
        v = self.visit(bind.expr, depth + 1)
        self.bind_name.pop()
        return v

    def builtin(self, builtin: Builtin, depth=0) -> Any:
        trace(depth, 'Builtin: {}'.format(builtin))
        return '{}'.format(builtin.name)

    def arrow(self, arrow: Arrow, depth=0) -> Any:
        trace(depth, 'arrow')
        args = ', '.join([self.visit(arg, depth + 1) for arg in arrow.args])
        return '({}) -> {}'.format(args, self.visit(arrow.return_type, depth + 1))

    def call(self, call: Call, depth=0) -> Any:
        trace(depth, 'call')
        fun = self.visit(call.function, depth + 1)

        args = []
        for arg in call.args:
            arg_val = self.visit(arg, depth + 1)
            if isinstance(arg, NamedArgument):
                args.append('{} = {}'.format(arg.name, arg_val))
            else:
                args.append(arg_val)

        args = ', '.join(args)
        return '{}({})'.format(fun, args)

    def binary_operator(self, call: BinaryOperator, depth=0) -> Any:
        trace(depth, 'binary_call')
        return '{} {} {}'.format(
            self.visit(call.args[0], depth + 1),
            self.visit(call.function, depth + 1),
            self.visit(call.args[1], depth + 1))

    def unary_operator(self, call: UnaryOperator, depth=0) -> Any:
        trace(depth, 'unary_call')
        return '{}({})'.format(self.visit(call.function, depth + 1), self.visit(call.args[0], depth + 1))

    def union(self, union: Union, depth=0) -> Any:
        trace(depth, 'union')
        args = ', '.join(['{}: {}'.format(m.name, self.visit(m.type, depth + 1)) for m in union.members])
        return 'Union({})'.format(args)

    def struct(self, union: Struct, depth=0) -> Any:
        trace(depth, 'struct')
        args = ', '.join(['{}: {}'.format(m.name, self.visit(m.type, depth + 1)) for m in union.members])
        return 'Struct({})'.format(args)


def to_string(expr, ctx=Scope()):
    return ToStringV.run(expr, ctx)


def print_expr(expr, ctx=Scope()):
    print(ToStringV.run(expr, ctx))


