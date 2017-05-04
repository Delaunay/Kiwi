#undef printd
#undef printdt
#undef dumptok
#undef freeprint
#undef debug
#undef debug_if

#ifdef KIWI_DEBUG
#   include <iostream>
#   define printd(x)     std::cout << x << std::endl
#   define printdt(x, y) std::cout << x; print(std::cout, y); std::cout << std::endl;
#   define dumptok(tol)  tok.debug_print(std::cout) << std::endl;
#   define freeprint(x)  std::cout << "Freeing: "; print(std::cout, x); \
                         std::cout << std::endl;
#   define debug(x)      x
#   define debug_if(cond, x) {if (cond) {x;}}
#else
#   define printd(x)
#   define printdt(x, y)
#   define dumptok(tol)
#   define freeprint(x)
#   define debug(x)
#   define debug_if(cond, x)
#endif


