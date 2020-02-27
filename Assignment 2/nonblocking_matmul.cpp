#include "nonblocking_matmul.h"
using namespace std;

void nonblocking_matmul(int argc, char**argv)
{
    int rank,size;
    int n = atoi(argv[1]);
    
    MPI_Status status;
    MPI_Request request;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 2 ) 
    {
        printf("Need at least two MPI tasks. Quitting...\n");
        MPI_Abort(MPI_COMM_WORLD, 0);
        exit(1);
    }

    if(rank == 0)
    {
        int offset,rows;
        
        float* a, *b, *c, *d;
        a = (float*)malloc(sizeof(float)*32*n);
        b = (float*)malloc(sizeof(float)*32*n);
        c = (float*)malloc(sizeof(float)*n*n);
        d = (float*)malloc(sizeof(float)*n*n);

        for(int i=0;i<n;i++)
        {
            for(int j=0;j<32;j++)
            {
                a[i*32+j] = ((float) rand() / (RAND_MAX));
            }
        }

        for(int i=0;i<32;i++)
        {
            for(int j=0;j<n;j++)
            {
                b[i*n+j] = ((float) rand() / (RAND_MAX));
            }
        }

        double start = MPI_Wtime();

        rows = ((n-1)/(size-1))+1;
        offset=0;
        

        for(int dest=1;dest<size;dest++)
        {
            rows = (rows>n-offset)?(n-offset):rows;

            MPI_Isend(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD, &request);
            MPI_Isend(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD, &request);
            MPI_Isend(&a[offset*32], rows*32, MPI_FLOAT,dest,1, MPI_COMM_WORLD, &request);
            MPI_Isend(b, 32*n, MPI_FLOAT, dest, 1, MPI_COMM_WORLD, &request);
            offset = offset + rows;

        }

        MPI_Wait(&request, &status);
        for (int source=1; source<size; source++)
        {
            MPI_Irecv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &request);
            MPI_Irecv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

            MPI_Irecv(&c[offset*n], rows*n, MPI_FLOAT, source, 2, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

        }
        MPI_Wait(&request, &status);

        double finish = MPI_Wtime();
        printf("Parallel done in %lf seconds.\n", (double)(finish - start));

        double stsim = MPI_Wtime();

        Matrix_Multiply(a,b,d,n,32,n);

        double finsim = MPI_Wtime();

        printf("Serial done in %lf seconds.\n", (double)(finsim - stsim));
        if(IsEqual(c,d,n,n))
        {
            printf("Yes\n");
        }

    }
    if(rank!=0)
    {
        float* a, *b, *c;
        int offset,rows;
        int source = 0;
        MPI_Irecv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &request);
        MPI_Irecv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        
        a = (float*)malloc(sizeof(float)*rows*32);
        b = (float*)malloc(sizeof(float)*32*n);
        c = (float*)malloc(sizeof(float)*rows*n);

        MPI_Irecv(a, rows*32, MPI_FLOAT, source, 1, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        MPI_Irecv(b, 32*n, MPI_FLOAT, source, 1, MPI_COMM_WORLD, &request);

        MPI_Wait(&request, &status);

        /* Matrix multiplication */
        for(int i=0;i<rows;i++)
        {
            for(int j=0;j<n;j++)
            {
                c[i*n+j] = 0.0;
                for(int k=0;k<32;k++)
                {
                    c[i*n+j] += a[i*32+k]*b[k*n+j];
                }
            }
        }

        MPI_Isend(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        MPI_Isend(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        MPI_Isend(c, rows*n, MPI_FLOAT, 0, 2, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
    }

    MPI_Finalize();
}