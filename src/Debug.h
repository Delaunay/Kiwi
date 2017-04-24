#undef printd
#undef printdt
#undef dumptok
#undef freeprint

#ifdef KIWI_DEBUG
#   include <iostream>
#   define printd(x)     std::cout << x << std::endl
#   define printdt(x, y) std::cout << x; print(std::cout, y); std::cout << std::endl;
#   define dumptok(tol)  tok.debug_print(std::cout) << std::endl;
#   define freeprint(x)  std::cout << "Freeing: "; print(std::cout, x); \
                         std::cout << std::endl;
#else
#   define printd(x)
#   define printdt(x, y)
#   define dumptok(tol)
#   define freeprint(x)

#endif


