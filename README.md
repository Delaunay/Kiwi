Kiwi
====

## Compilation Steps

    git clone https://Setepenre@bitbucket.org/Setepenre/kiwi.git
    cd kiwi
    git submodule update --init
    mkdir build
    cd build
    cmake ../
    make

Tested with MSVC 19 et GCC 6

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

* Distributed System

Kiwi is a compiler/server





