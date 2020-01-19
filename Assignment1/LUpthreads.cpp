#include "library.h"
#include "defs.h"

vvi A,P,L,U;

struct thread_data
{
    int tin;
    int tout;
    int tjt;
    int tnt;
    int tot;
};

void* thread_routine(void* arg)
{
    thread_data* data = (thread_data*)arg;
    for(int i=data->tin; i < (data->tout); ++i)
    {
        for(int j = (data->tjt); j < (data->tnt); ++j)
        {
            A[i][j] = A[i][j] - L[i][data->tjt]*U[data->tjt][j];
        }

    }
}

void LUpthreads(int num_threads, int n, vvi &Ap, vvi &Pp, vvi &Lp, vvi &Up)
{
    A=Ap;
    P=Pp;
    L=Lp;
    U=Up;
    pthread_t threads[num_threads];
    thread_data *td;
	td= new thread_data[num_threads];

    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
            P[i][j] = (i==j);
    }

    for(int k=0;k<n;k++)
    {
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

        for (int i = k+1; i<n; i++)
        {
            L[i][k] = A[i][k]/U[k][k];
            U[k][i] = A[k][i];
        }

        int st=0;
        int y=((n-1)/num_threads)+1;
        int a=0;
        for(a=0;a<num_threads;a++)
        {
            td[a].tin=st;
            td[a].tout=min(st+y,n);
		    td[a].tot = y;
		    td[a].tjt = k;
            td[a].tnt = n;

            int thread_error = pthread_create(&threads[a],NULL ,thread_routine, (void*) &td[a]);
            if(thread_error)
                cout<<"Thread error\n";
            
            st+=y;

            if(st>=n)
                break;
        }

        for(int j=0;j<a;++j)
		    pthread_join(threads[j], NULL);
    }
}