

def trace(depth, message, mode):
    if mode:
        print(''.join(['|' if i % 2 else ':' for i in range(depth)]) + '-> {}'.format(message))

