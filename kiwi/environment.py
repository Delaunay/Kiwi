from kiwi.expressions import *
from dataclasses import dataclass
from dataclasses import field


@dataclass
class Scope:
    previous: 'Scope' = None
    name_index: Dict[str, int] = field(default_factory=dict)
    index_name: Dict[int, str] = field(default_factory=dict)     # for debugging
    expressions: List[Expression] = field(default_factory=list)
    offset: int = 0

    def insert(self, item: Any):
        self.expressions.append(item)

    def insert_binding(self, name: str, item: Any):
        self.name_index[name] = len(self.expressions)
        self.index_name[len(self.expressions) + self.offset] = name
        self.expressions.append(item)

    def __len__(self):
        return self.offset + len(self.expressions)

    def get_index(self, name):
        if name in self.name_index:
            return self.name_index[name] + self.offset
        elif self.previous is not None:
            return self.previous.get_index(name)
        else:
            # TODO: Warning or raise exception
            return -1

    def get_expression(self, index):
        if index > self.offset:
            rindex = len(self) - index
            return self.expressions[rindex]
        elif self.previous is not None:
            return self.previous.get_expression(index)
        else:
            # TODO: Warning or raise exception
            return None

    def get_name(self, index):
        if index in self.index_name:
            return self.index_name[index]
        elif self.previous is not None:
            return self.previous.get_name(index)
        else:
            # TODO: Warning or raise exception
            return None

    def enter_scope(self) -> 'Scope':
        return Scope(previous=self, offset=len(self.expressions) + self.offset)

    def exit_scope(self) -> 'Scope':
        if self.previous is None:
            return self

        return self.previous

    def dump(self):
        if self.previous is not None:
            self.previous.dump()

        l = len(self)
        for i, expr in enumerate(self.expressions):
            print('{:4d} | {:<20} | {:<20}'.format(i + self.offset, self.get_name(i + self.offset), str(expr)))


def environment_test():
    scope = Scope()

    scope.insert_binding('a', 1)
    scope.insert_binding('b', 2)
    scope.insert_binding('c', 3)
    scope.insert_binding('d', 4)

    print('c', scope.get_index('c'))

    scope.dump()
    print('-' * 80)

    scope = scope.enter_scope()

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