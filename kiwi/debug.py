

def trace(depth, message, mode, name):
    if mode:
        print(''.join([name] + [':' if i % 2 else '|' for i in range(depth)]) + '+-> {}'.format(message))

