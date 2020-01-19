#include "library.h"
#include "defs.h"

void LUopenMP(int num_threads, int n, vvi &A, vvi &P, vvi &L, vvi &U)
{
#pragma omp parallel for
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
            P[i][j] = (i==j);
    }

#pragma omp parallel for 
    for(int i=0;i<n;i++)
    {
        int maxi = i;

#pragma omp critical
        for(int j=i;j<n;j++)
        {
            if(abs(A[j][i]) > abs(A[maxi][i])) 
                maxi=j;
        }

        if(A[maxi][i]==0)
        {
            cout<<"Singular Matrix";
            return ;
        }

#pragma omp critical
        if(maxi != i)
        {
            for(int k=0;k<n;k++)
            {
                swap(P[i][k], P[maxi][k]);
                swap(A[i][k], A[maxi][k]);

                if(k<i)
                    swap(L[i][k], L[maxi][k]);

            }
        }

        U[i][i] = A[i][i];

        for (int k = i+1; k<n; k++)
        {
            L[k][i] = A[k][i]/U[i][i];
            U[i][k] = A[i][k];
        }

        for(int k=i+1; k<n; k++)
        {
            for(int j=i+1; j<n; j++)
            {
                A[k][j] = A[k][j] - L[k][i]*U[k][j];
            }
        }
    }
}