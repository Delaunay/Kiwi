Kiwi
====


All expressions are anonymous by default. To name them the bind operator need to be used.

```
# Makes a struct and bind it to Point
Point = struct(x: Float, y: Float)

o = Point(2.2, 3.3)

# makes a union and bind it to 
MaybeFloat = union(x: Float)

no_float = MaybeFloat()
some_float = MaybeFloat(x = 2.2)

# Unpacking #1
o match 
 | Point(x, y) =>
 
# Unpacking #2
# That might be easy to implement
# this is just a bind expression that is specialized for structs
x, y = o
 
# Pattern Matching
maybe_float match 
 | MaybeFloat(x = x) =>
 | None => 
 
FloatOrInt = union(f: Float, i: Int)

foi match 
 | FloatOrInt(f = x) => 
 | FloatOrInt(i = x) => 
 | None => 
 
# Switch type of match
val match 
 | 1 =>
 | 2 => 
 | 3 =>
 | None => 

```


