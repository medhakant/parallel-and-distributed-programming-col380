#include "library.h"
#include "defs.h"



void LUopenMP(int thread_count, int n, vvi &A, vvi &P, vvi &L, vvi &U)
{
    // omp_set_num_threads(thread_count);
    // cout<<num_threads<<endl;

    // cout<<omp_get_num_threads()<<endl;
#pragma omp parallel for num_threads(thread_count)
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
            P[i][j] = (i==j);
    }

// #pragma omp parallel for 
    for(int k=0;k<n;k++)
    {
        if(k==0)
            cout<<omp_get_num_threads()<<endl;
        int maxi = k;

        for(int i=k;i<n;i++)
        {
            if(abs(A[i][k]) > abs(A[maxi][k])) 
                maxi=i;
        }

        if(maxi != k)
        {
            for(int i=0;i<n;i++)
            {
                swap(P[k][i], P[maxi][i]);
                swap(A[k][i], A[maxi][i]);

                if(i<k)
                    swap(L[k][i], L[maxi][i]);

            }
        }

        U[k][k] = A[k][k];

// #pragma omp parallel for 
        for (int i = k+1; i<n; i++)
        {
            L[i][k] = A[i][k]/U[k][k];
            U[k][i] = A[k][i];
        }
#pragma omp parallel for num_threads(thread_count)
        for(int i=k+1; i<n; i++)
        {
            for(int j=k+1; j<n; j++)
            {
                A[i][j] = A[i][j] - L[i][k]*U[k][j];
            }
        }
    }
}