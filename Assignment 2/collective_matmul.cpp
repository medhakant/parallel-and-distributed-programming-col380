#include "collective_matmul.h"
using namespace std;

void collective_matmul(int argc, char**argv){
    int rank, size;
	int n;
	int partrow,fracrow;
	float *bufa,*bufc;
	float *a,*b,*c,*d;
	float sum = 0;
	int i,j,k;
	double start,finish;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //ID of processor
	MPI_Comm_size(MPI_COMM_WORLD, &size); //amount of processor

	if(rank==0){

		n = atoi(argv[1]);

        partrow = n/size;
        fracrow = n%size;

        //allocate matrix
        a = (float*)malloc(n * 32 * sizeof(float));
        b = (float*)malloc(32 * n * sizeof(float));
        c = (float*)malloc(n * n * sizeof(float));
        d = (float*)malloc(n * n * sizeof(float));

        for(i=0;i<n;i++)
        {
            for(j=0;j<32;j++)
            {
                a[i*32+j] = ((float) rand() / (RAND_MAX));
            }
        }

        for(i=0;i<32;i++)
        {
            for(j=0;j<n;j++)
            {
                b[i*n+j] = ((float) rand() / (RAND_MAX));
            }
        }
	}

	start = MPI_Wtime();

	MPI_Bcast(&partrow,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);

	if (rank!=0)
	{
		b = (float*)malloc(32 * n * sizeof(float));
		bufa = (float*)malloc(partrow * 32 * sizeof(float));
	}

	MPI_Bcast(&b[0],32*n,MPI_FLOAT,0,MPI_COMM_WORLD);

	bufc = (float*)malloc(partrow * n * sizeof(float));

	if(rank==0){
		//send matrix A
		for (i=1; i<size; i++) {
			MPI_Send(&a[partrow*32*i],partrow*32, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}
	}
	else{ //other rank

		MPI_Recv(&bufa[0],partrow*32, MPI_FLOAT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		//multiplication in other rank
		for(i=0;i<partrow;i++){ //shift down row in A
			for(j=0; j < n; j++){ //shift down row in B
                sum = 0;
				for(k = 0 ; k < 32; k++){
					sum+=bufa[(i*32)+k]*b[(k*n)+j];
				}
				bufc[(i*n)+j]=sum;
			}
		}
		MPI_Send(&bufc[0],partrow*n, MPI_FLOAT, 0, 123, MPI_COMM_WORLD);
	}

	if(rank==0) //rank 0
	{
		//multiplication in rank 0
		for(i=0;i<partrow;i++){
			for(j=0; j < n; j++){
                sum = 0;
				for(k = 0 ; k < 32; k++){
					sum+=a[(i*32)+k]*b[(k*n)+j];
				}
				c[(i*n)+j]=sum;
			}
		}

		if (fracrow!=0)
		{
			//fraction multiplication in rank 0
			for(i=partrow*size;i<(partrow*size)+fracrow;i++){
				for(j=0; j < n; j++){
                    sum = 0;
					for(k = 0 ; k < 32; k++){
						sum+=a[(i*32)+k]*b[(k*n)+j];
					}
					c[(i*n)+j]=sum;
				}
			}
		}

		for (i=1; i<size; i++) { //other PART from other rank
			MPI_Recv(&c[partrow*n*i],partrow*n, MPI_FLOAT, i, 123, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}

		finish = MPI_Wtime();
		printf("Parallel done in %lf seconds.\n", finish - start);

        start = MPI_Wtime();

        Matrix_Multiply(a,b,d,n,32,n);

        finish = MPI_Wtime();

        printf("Serial done in %lf seconds.\n", (double)(finish - start));
        if(IsEqual(c,d,n,n))
        {
            printf("Yes\n");
        }
	}

	MPI_Finalize();
}