

# Pattern Matching

## Unpacking


    Vec2 = struct(x: Float, y: Float)
    
    
    def v1_add(a: Vec2) -> Float:
        switch a:
            case (x, y):
                return x + y
    
    def v2_add(a: Vec2) -> Float:
        (x, y) = a
        return x + y
        
     v = Vec2(2, 2)
     
     add(v) # == 4

## Value Branching

    def if(a: Bool, then: Expr, else: Expr) -> Expr:
        switch a:
            case True:
                return then
               
            case False:
                return else

    if(True, 1, 2)  # == 1
    if(False, 1, 2) # == 2


## Type Dispatch
    
    FloatOrInt = union(float: Float, int: Int)
    
    def get_float(a: FloatOrInt, default: Float = 0) -> Float:
        switch a: 
            case float(value):
                return value
               
            case int( _ ):
                return default
    
    
    fa = FloatOrInt(float = 2.0)
    ia = FloatOrInt(int = 1)
    
    get_float(fa, 0) # == 2
    get_float(ia, 3) # == 3