#makefile to compile the files
#creates an executable named ludecomp
CC = g++
LIBS = -fopenmp -pthread
CFLAGS = 
OBJ = ludecomp.o mainfile.o


ludecomp: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *~ ludecomp
