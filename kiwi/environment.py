from kiwi.expressions import *
from dataclasses import dataclass
from dataclasses import field


@dataclass
class ReferenceNotFound(Exception):
    ref: VariableRef


@dataclass
class Scope:
    name: str = None
    expressions: Dict[str, Expression] = field(default_factory=dict)
    order: List[str] = field(default_factory=list)
    offset: int = 0
    previous: 'Scope' = None

    def insert_binding(self, name, item: Expression) -> 'Scope':
        self.order.append(name)
        self.expressions[name] = item
        return self

    def __len__(self):
        return len(self.expressions) + self.offset

    def get_expression(self, ref: VariableRef, depth=0):
        if ref.name in self.expressions:
            return self.expressions[ref.name]

        if self.previous is None:
            raise ReferenceNotFound(ref)

        return self.previous.get_expression(ref, depth + 1)

    def enter_scope(self, name=None) -> 'Scope':
        return Scope(name=name, previous=self, offset=len(self.expressions) + self.offset)

    def exit_scope(self) -> 'Scope':
        if self.previous is None:
            return self
        return self.previous

    def get_index(self, name):
        if name in self.order:
            # the index could be inserted in the dict so we can do a simple lookup like get_expression
            # Does not matter this class is temporary
            for i, s in enumerate(self.order):
                if s == name:
                    return i, self.expressions[s]
        elif self.previous is not None:
            return self.previous.get_index(name)

        return -1, None

    def dump(self):
        if self.previous is not None:
            self.previous.dump()

        print('{} Offset: {}'.format(self.name, self.offset))
        for i in self.order:
            print(self.expressions[i])


# FIXME Indices are broken
# we use explicit reference instead at the moment
# but this will not hold for the Evaluator
# debruijn index are bitches
@dataclass
class ScopePrime:
    name: str = None
    previous: 'Scope' = None
    name_index: Dict[str, int] = field(default_factory=dict)
    index_name: Dict[int, str] = field(default_factory=dict)     # for debugging
    expressions: List[Expression] = field(default_factory=list)
    offset: int = 0

    def insert(self, item: Any):
        item.ctx_index = len(self)
        self.expressions.append(item)

    def insert_binding(self, name: str, item: Any):
        self.name_index[name] = len(self)
        self.index_name[len(self)] = name
        self.expressions.append(item)

    def __len__(self):
        return self.offset + len(self.expressions)

    def get_index(self, name) -> Tuple[int, Expression]:
        if name in self.name_index:
            idx = self.name_index[name]
            return idx, self.expressions[idx - self.offset]

        elif self.previous is not None:
            return self.previous.get_index(name)
        else:
            print('get_index: `{}` not found'.format(name))
            # TODO: Warning or raise exception
            return -1, None

    def get_expression(self, ref: VariableRef, depth=0, prev=0):
        # The reference is in our Scope
        reverse_index = ref.size - ref.index
        print('=' * 60)
        print(ref)
        print('Passed Values', prev)
        print('reverse_index', reverse_index)
        print('Scope Offset', self.offset)
        print('Index', ref.index)
        print('len ', len(self.expressions))
        index = reverse_index - prev    # len(self.expressions) + self.offset - ref.index
        print('Rectified index', index)
        print(self.previous)

        if prev == 0:
            self.dump()

        if ref.index > self.offset:
            r = self.expressions[-index]
            print(r)
            return self.expressions[-index]
        else:
            print('Back')
            return self.previous.get_expression(ref, depth + 1, prev=len(self.expressions) + prev)

        offset = ref.size - ref.index

        # Number of element in this context
        local_size = len(self) - self.offset

        # print(' ' * depth + 'size={} original_offset={}'.format(local_size, offset))
        if offset <= local_size:
            # print(' ' * depth + str(offset))
            return self.expressions[-offset]

        elif self.previous is not None:
            return self.previous.get_expression(ref, depth + 1)
        else:
            print('get_expression: `{}` not found'.format(ref))
            # TODO: Warning or raise exception
            return None

    def get_name(self, index):
        if index in self.index_name:
            return self.index_name[index]
        elif self.previous is not None:
            return self.previous.get_name(index)
        else:
            print('get_name: `{}` not found'.format(index))
            # TODO: Warning or raise exception
            return None

    def enter_scope(self, name=None) -> 'Scope':
        #if len(self.expressions) == 0:
        #    return self

        print('Scope Enter')
        return Scope(name=name, previous=self, offset=len(self.expressions) + self.offset)

    def exit_scope(self) -> 'Scope':
        print('Scope Exit')
        if self.previous is None:
            return self

        return self.previous

    def dump(self, depth=0, prev=0):
        if depth == 0:
            print('=' * 80)

        if self.previous is not None:
            self.previous.dump(depth + 1, prev=len(self.expressions) + prev)

        print('{} Offset: {}'.format(self.name, self.offset))
        local_length = len(self.expressions)
        for i, expr in enumerate(self.expressions):
            rep = repr(expr).split('\n')

            print('{:4d} {:4d} | {:<20} | {:<20}'.format(local_length - i + prev, i + self.offset, self.get_name(i + self.offset), rep[0]))
            rep = rep[1:]
            for line in rep:
                print('          |                      | {}'.format(line))

        if depth == 0:
            print('=' * 80)


def environment_test():
    scope = Scope()

    scope.insert_binding('a', 1)
    scope.insert_binding('b', 2)
    scope.insert_binding('c', 3)
    scope.insert_binding('d', 4)

    print('c', scope.get_index('c'))

    scope.dump()
    print('-' * 80)

    scope = scope.enter_scope('new_test_scope')

    scope.insert_binding('a', 5)
    scope.insert_binding('b', 6)
    scope.insert_binding('c', 7)
    scope.insert_binding('d', 8)

    print('c', scope.get_index('c'))

    scope.dump()
    print('-' * 80)

    scope = scope.exit_scope()
    scope.dump()
    print('-' * 80)


if __name__ == '__main__':
    environment_test()