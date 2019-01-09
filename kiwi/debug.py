

def trace(depth, message, mode, name):
    if mode:
        print(''.join([name] + ['{:3d} '.format(depth)] + [':' if i % 2 else '|' for i in range(depth)]) + '+-> {}'.format(message))

