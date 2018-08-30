Kiwi
====

Kiwi is an experimental language. At the moment I mainly try stuff.

## Compilation Steps

    git clone https://Setepenre@bitbucket.org/Setepenre/kiwi.git
    cd kiwi
    git submodule update --init
    mkdir build
    cd build
    cmake ../
    make

Tested with MSVC 19

## Kiwi Philosophy

Kiwi tries to help the programmer express himself and his intention without hamper flexibility to do so.
    
## Kiwi AST

    * Statement
        * Expression        High Level Instructions
            * value                                    
                * object
                * int
                * double
                * ...
            * call                                  Unconditional Jump
                *   binary call `a * b`             // A lot of functions have only 1 or 2 arguments
                *   unary  call ` - a `             // plus it will allow the user to define custom operators
                * function call (generic)           // like matlab .* or .+ 
            * match                                 Conditional Jump
            * block                                 Sequence of Instructions
            * error                                 Represent an erroneous state (Might be something like Rust Result<T, E>)
                                                    Hopefully making it as invisible as possible and allowing users to have an error handling facility in between exceptions and error codes 
        * Definition        Reusable components
            * function                              Reusable Subtree
            * macros                                Expression generator - Compile time function
            * Union                                 A  or B can be extracted using match 
            * Struct                                A and B
        * Type              Code Meta data
            * Builtin                               default types injected by the compiler
            * Arrow                                 Function Type (a: A) -> B
            * Union                                 Reference to a union def
            * Struct                                Reference to a struct def
        * Pending           User Input

Example    

    def my_fun(a: A, b: B) -> C:
        def wrap(x: X) -> Z:
            return x * a + b
            
        return wrap

AST Break down. The initial AST is pretty generic with very few constraint and what can be where.
Basically everything is a statement. 

While types can be anything initially, will have to be resolved to a Type at compile time.
If not it will cause an error.

    Definition function( 
        name        = my_fun           : Identifier
        args        = [(a, A), (b, B)] : Array<(Statement, Statement)> 
        return_type = C                : Statement 
        body                           : Statement
            Expression block(
                Definition function(
                    name         = wrap     : Identifier
                    args         = [(x, X)] : Array<(Statement, Statement)> 
                    return_type = Z         : Statement
                    body                    : Statement
                        Expression unary call(
                            call_name = return : Statement
                            args      = [binary_call(+, binary_call(*, x, a), b)] : Array<Statement>
                    )
                ),
                Expression unary call(
                    name = return    : Statement
                    args = [wrap]    : Array<Statement>            
                )
            )
        )

Meta Types

    struct Point(T: Type):
        x: T
        y: T
        
    union Either(R: Type, L: Type):
        left: L
        right: R

Making everything being a statement will allow users to compute types at compile time and annotate their code with
arbitrary expression, like `x: make_type(x, y, z)`.

We could define something like:

    struct Tuple(X: Type, Y: Type):
        x: X
        y: Y
        
    union Variant(X: Type, Y: Type):
        x: X
        y: Y

    def operator * (X: Type, Y: Type): Struct =
        return Tuple(X, Y)

    def operator | (X: Type, Y: Type): Struct =
        return Variant(X, Y)
        
    struct MyData:
        t: Int * Float    => Function Call resolved to a Tuple
        v: Int | Float    => Function Call resolved to a Variant
        
    # Extract meta types 
    def get_x(tuple: Tuple(A: Type, Float)): A 
        return tuple.x
        
NB: because everything is a statement, it makes Kiwi type system dependent.
See example below where the value N is part of the type.

    Struct Vector(T: Type, N: Int):
        data : T[N]
        
        
## Reflection as code gen

I personnaly do not see the attraction of reflection as a runtime feature.
Kiwi use macros to expose the source code to the user.
For example, Kiwi does not differenciate between a Tuple and a Reccord because
both of them ater interchangeable using macros.


    struct Whatever:
        x: X
        y: Y

    x = Whatever();
    x.x = 1
    x.y = 2
        
    # Macros that are executed at compile time
    get_fields(x)       <-  [(x, X), (y, Y)]
    get_field(0, x)     <- 
    get_field("x", x)
    
    # Get the parsed AST back
    # This means you can write a bunch of code
    # get its ASt and generate a bunch of more code based on that
    # this should make SQL/Data Serialization very straight forward and
    # nicely integrated with the language
    # 
    # Additionally something like autodiff for ML should also be possible
    get_ast(x)
    
    # This is definetly something I want to
    get_llvm_ir(x)

 
## Kiwi Specifications

* Type checking is done through Z3. Typing are program the program constraint.
Z3 checks that none of constraint are broken providing the programer with some guarantees.
The guarantees provided by Kiwi are only as good as the constraint the programmer put on his program.
Additional constraints might limit the generality of the code but provide better security and better chance for optimization.


Kiwi allows you to write Z3 constraint as type annotations
The `get_value` function bellow guarantees that no out of bounds error can occur when this function is called.
When the guarantee cannot be satisfied a compile time error is written back. You can then  choose to refactor
your code inorder to provide the necessary guarantee or use the unsafe version.


    def get_value(array: Array<T, N>, (index : Int) < N) -> Int:
        return array[index];
        
        
    array = Array(T=Double, N=10)
    
    get_value(array, 10) => returns Error("Broken Constraint ((Index: 11)  : Int) < (N: 10)")
    get_value(array, 9)  => returns Success(0)
    

## Generic Computation Device Interface

* Computation Device                        Communication 
    * Cluster                             Ethernet / Sockets
        * Computer/VM                           Sockets 
        * CPU(s)                                RAM 
            * Cores                                  - 
                * Threads                         - 
        * GPU(s)                                VRAM
            * Wraps/Wavefront
    

## Distributed System

Kiwi as a compiler/server/executor

                                                             *-------------------> Kiwi Node
                                                             |
                                                             *-------------------> Kiwi Node
                                                             |
    * Kiwi Master ---- Send Serialized Sub Graph to Compute -*-------------------> Kiwi Node
                                                             |
                                                             *-------------------> Kiwi Node
                                                             |
                                                             *-------------------> Kiwi Node


# TIDE


TIDE or Tree-IDE is a new IDE concept in which the AST of a language itself is rendered to the screen
as opposed to traditional IDEs that render text which turns out to be code.

While the rendered AST can end up looking like text this new approach allows the IDE to be aware of
the language being rendered allowing a lot of cool features to be naturally builtin like:

* Precise highlighting: The IDE knows what each string is supposed to represent (function name, type, variable, etc...)
allowing us to specify different style for each entity. 

* Refactoring; changing a name will be the easiest thing in the world since modifying it in one place will
automatically update the naming everywhere else in the code base. You do not even have to worry about false positive!

* Diffing; TIDE will be able to provide you with a per function/class diffing

* Compiler Metadata: because TIDE is not a text editor per se it is not limited to rendering text but can also render
abitrary things including compiler metadata about the program.


# Kiwi Distributed File System and Job Scheduling

## Concept

Basically a kind of REST interface. 

    /kdfs/machine_id/data
    /kdfs/machine_id/jobs/logs/:job_name/:field_name
    /kdfs/machine_id/jobs/history/:job_name/:field_name
    /kdfs/machine_id/jobs/running/:job_name/:field_name
    /kdfs/machine_id/jobs/completed/:job_name/:field_name
    /kdfs/machine_id/jobs/new

    # Insert input data into a specific machine
    # Node that nothing prevent you from using the data from a different machine
    # IO is just going to be slower
    cat my_data.csv > /kdfs/machine_id/data/my_data.csv
    
    # Schedule a job on machine_id
    cat my_job_name.spec > /kdfs/machine_id/jobs/new
    
    # Show running jobs
    ls /kdfs/machine_id/jobs/running
    ls /kdfs/machine_id/jobs/running | grep my_job_name
    
    # get job logs
    cat /kdfs/machine_id/jobs/logs/my_job_name/stdout > log.stdout
    
    # Global Namespace
    # Allows you to read data from the cluster. KDFS will decide from which machine
    # might even be some sort of peer to peer transfer
    /kdfs/global/data
    /kdfs/global/jobs
    
## Functions


    myfun => Ref(Function(myfun))
    
                 Placeholders
              <------>  <------>                                        
    def myfun(a: Float, b: Float) -> Float:           
        return a + b            # a => Ref(Placeholder(a))
                                # b => Ref(Placeholder(b))
                                                      
                                                        
    







