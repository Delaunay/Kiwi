from kiwi.expressions import *


class LocalEnvironment:
    offset: int
    env: List[Expression] = []
    name_index: Dict[str, int] = {}

    def insert_binding(self, name: str, expr: Expression):
        self.env.append(expr)
        self.name_index[name] = len(self.env) - 1

    def insert(self, value: Expression):
        self.env.append(value)

    def find_index(self, name: str) -> int:
        return self.name_index.get(name)

    def find_expr(self, index: int) -> Expression:
        # print(self.env)
        return self.env[index]


class Context:
    env: List[LocalEnvironment] = [LocalEnvironment()]
    glob: List[Expression] = []

    def insert_binding(self, name: str, expr: Expression):
        self.env[-1].insert_binding(name, expr)
        self.glob.append(expr)
        return VariableRef(len(self.glob) - 1)

    def insert(self, expr: Expression):
        self.env[-1].insert(expr)
        self.glob.append(expr)

    def find_index(self, name: str) -> int:
        for ctx in reversed(self.env):
            obj = ctx.find_index(name)
            if obj is not None:
                return obj

    def reference(self, name):
        index = self.find_index(name)
        return VariableRef(index)

    def find_expr(self, index: int) -> Expression:
        return self.glob[index]

        # for ctx in reversed(self.env):
        #     obj = ctx.find_expr(index)
        #     if obj is not None:
        #         return obj

    def push_env(self):
        self.env.append(LocalEnvironment())
        self.env[-1].offset = len(self.env[-2].env)

    def pop_env(self):
        self.env.pop()


class ToStringV(Visitor):
    def __init__(self, ctx = Context()):
        super(ToStringV).__init__()
        self.env_stack: Context = ctx

    @staticmethod
    def run(expr: Expression):
        return ToStringV().visit(expr)

    def value(self, val: Value) -> Any:
        return '{}: {}'.format(str(val.value), self.visit(val.type))

    def variable(self, var: Variable) -> Any:
        self.env_stack.insert_binding(var.name, var)
        return '{}'.format(str(var.name))

    def reference(self, ref: VariableRef) -> Any:
        expr = self.env_stack.find_expr(ref.index)
        return self.visit(expr)

    def function(self, fun: Function) -> Any:
        self.env_stack.push_env()
        args = ', '.join([self.visit(arg) for arg in fun.args])

        if fun.is_lambda:
            fun_str = 'lambda {}: {{ {} }}'.format(
                args, self.visit(fun.body))

            self.env_stack.pop_env()
            return fun_str

        fun_str = '({}) -> {} {{ {} }}'.format(
            args,
            self.visit(fun.return_type),
            self.visit(fun.body))

        self.env_stack.pop_env()
        return fun_str

    def match(self, match: Match) -> Any:
        raise NotImplemented

    def conditional(self, cond: Conditional) -> Any:
        raise NotImplemented

    def builtin(self, builtin: Builtin) -> Any:
        print(builtin)
        return '{}'.format(builtin.name)

    def arrow(self, arrow: Arrow) -> Any:
        args = ', '.join([self.visit(arg) for arg in arrow.args])
        return '({}) -> {}'.format(args, self.visit(arrow.return_type))

    def call(self, call: Call) -> Any:
        args = ','.join([self.visit(arg) for arg in call.args])
        return '{}({})'.format(self.visit(call.function), args)

    def binary_operator(self, call: BinaryOperator) -> Any:
        print(call)
        return '{} {} {}'.format(self.visit(call.args[0]), self.visit(call.function), self.visit(call.args[1]))

    def unary_operator(self, call: UnaryOperator) -> Any:
        print(call)
        return '{}({})'.format(self.visit(call.function), self.visit(call.args[0]))


def print_expr(expr):
    print(ToStringV().run(expr))

if __name__ == '__main__':
    ctx = Context()

    type_type = ctx.insert_binding('Type', Builtin('Type', None))
    float_type = ctx.insert_binding('Float', Builtin('Float', type_type))
    return_op = ctx.insert_binding('return', Builtin('return', Arrow([type_type], type_type)))

    add_type = Arrow([float_type, float_type], float_type)
    add_fun = ctx.insert_binding('+', Builtin('+', add_type))

    fun = Function(
        [Variable('x', float_type), Variable('y', float_type)], float_type,
        UnaryOperator(
            return_op,
            BinaryOperator(
                add_fun,
                # Todo remove hardcoded ref
                VariableRef(5),
                VariableRef(6)
            )
        ), is_lambda=True
    )

    a = ctx.insert_binding('add2', fun)
    print(a)
    val = Value('abc', Builtin('String', ctx.reference('Type')))

    print(val)
    print_expr(val)
    print_expr(add_type)

    print(fun)
    print_expr(fun)



