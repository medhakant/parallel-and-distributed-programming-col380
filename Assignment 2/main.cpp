#include <bits/stdc++.h>
#include "mpi.h"
#include "blocking_matmul.h"
#include "nonblocking_matmul.h"
#include "collective_matmul.h"

void Matrix_Multiply(float *A, float *B, float *C, int m, int n, int p){
	int i, j, k;
	for (i = 0; i < m; i++){
		for (j = 0; j < p; j++){
			C[i*p + j] = 0.0;
			for (k = 0; k < n; k++)
				C[i*p + j] += A[i*n + k] * B[k*p + j];
		}
	}
}

int IsEqual(float *A, float *B, int m, int n)
{
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            if(A[i*n+j] != B[i*n+j])
                return 0;
        }
    }

    return 1;
}

int print_mat(float *A, int m, int n)
{
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            printf("%f ",A[i*n+j]);
        }
    }

    return 1;
}

int main(int argc, char**argv)
{
    int mode = stoi(argv[2]);
    if(mode==0)
        blocking_matmul(argc, argv);
    else if(mode==1)
        nonblocking_matmul(argc, argv);
    else
        collective_matmul(argc,argv);
    
}