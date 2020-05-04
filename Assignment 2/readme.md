### Assignment 2: Part 1

In this assignment we had to perform martrix multiplication using parallel execution between processors using MPI to communicate between the processors.

To compile the program, simply run the makefile. The makefile would produce an executable **matmul**. To compile the program with O3 optimisation, uncomment the O3 in **CFLAGS** in the makefile.

To run the program use:
```bash
mpirun -np [Nubmer of processor to use] matmul [matrix size] [parallel mode]
```
The *Number of processor to use* should be less than or equal to the number of processors your system has. Also, there are no restriction on matrix size. It doesn't necessarily have to be a perfectly divisible by the number of processor specified. The *parallel mode* can be 0,1,2.

- 0 > Blocking P2P Communication
- 1 > Non-Blocking P2P Communication
- 2 > Collective Communication 
