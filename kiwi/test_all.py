import sys
import os

sys.path.insert(0, os.curdir)

if __name__ == '__main__':

    import kiwi.test_add

    import kiwi.test_match
    import kiwi.test_trace

    import kiwi.test_interpreter

    kiwi.test_add.main()
    kiwi.test_match.main()
    kiwi.test_trace.main()

    kiwi.test_interpreter.main()

    import kiwi.test_user_types
    kiwi.test_user_types.main()

    import kiwi.test_codegen
    kiwi.test_codegen.main()
