#makefile to compile the files
#creates an executable named ludecomp
CC = mpic++
LIBS = 
CFLAGS = -O3
FILES = nonblocking_matmul.cpp blocking_matmul.cpp collective_matmul.cpp main.cpp


matmul: $(FILES)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *~ matmul

