To compile a pagerank algorithm, go to the directory and 
run the makefile and it would produce an executable with 
the name- mr-pr-cpp.o and pr-cpp.o for the code that doesn't 
use map reduce. To run the non-MPI verson, type 
./mr-pr-cpp.o $infile -o $outfile
and for the MPI verson, run mpirun -np 1 ./mr-pr-cpp.o $infile -o $outfile.
The code in the difference folder can be used to calculate 
the difference generated from two pagerank algorithms.
