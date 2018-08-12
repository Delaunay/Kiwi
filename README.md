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
		
		
	array = Array(T=Double, N=10)
	
	get_value(array, 10) => returns Error("Broken Constraint ((Index: 11)  : Int) < (N: 10)")
	get_value(array, 9)  => returns Success(0)

	
## Generic Computation Device Interface

* Computation Device						Communication 
	* Cluster 							Ethernet / Sockets
		* Computer/VM					       Sockets 
		* CPU(s)						        RAM 
			* Cores							 	 - 
				* Threads						 - 
		* GPU(s)							    VRAM
			* Wraps/Wavefront
	

## Distributed System

Kiwi as a compiler/server/executor

															 *-------------------> Kiwi Node
															 |
															 *-------------------> Kiwi Node
															 |
	* Kiwi Master 	-- Send Serialized Sub Graph to Compute -*-------------------> Kiwi Node
															 |
															 *-------------------> Kiwi Node
															 |
															 *-------------------> Kiwi Node


TIDE
====

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



Kiwi Distributed File System and Job Scheduling
===============================================

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
	ls /kdfs/machine_id/jobs/running | gerp my_job_name
	
	# get job logs
	cat /kdfs/machine_id/jobs/logs/my_job_name/stdout > log.stdout







