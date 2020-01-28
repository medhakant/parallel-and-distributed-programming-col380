#include "ludecomp.h"
using namespace std;


// this function runs on each individual thread
void* thread_routine(void* arg)
{
    // initialise thread data
    thread_data* data = (thread_data*)arg;
    for(int i=data->tin; i < (data->tout); ++i)
    {
        for(int j = (data->tjt); j < (data->tnt); ++j)
        {
            data->A[i][j] = data->A[i][j] - data->L[i][(data->tjt)-1]*data->U[(data->tjt)-1][j];
        }

    }
    pthread_exit(NULL);
}

// this function uses pthreads to compute the LU decomposition using pthreads
void lupthreads(int n,double** a,double** p,double** l,double** u,int* error,int threads){

    // deifne pthreads
    pthread_t pthr[threads];
        thread_data *td;
        td= new thread_data[threads];

    for(int k=0;k<n;k++){
        double max = 0;
        int k_;        

        // find the max index
        for(int i=k;i<n;i++){
            if(max < abs(a[i][k])){
                max=abs(a[i][k]);
                k_=i;
            }
        }

        // if max==0, set error and return
        if(max==0){
            *error = -1;
            return;
        }else{
            swap(a[k], a[k_]);
            swap(p[k], p[k_]);
            for(int i=0;i<k-1;i++){
                swap(l[k][i], l[k_][i]);
            }
        }

        u[k][k] = a[k][k];
        

        for (int i = k; i<n; i++){
            l[i][k] = a[i][k]/u[k][k];
            u[k][i] = a[k][i];
        }

        
        int st=k+1;
        int y=((n-k-1)/threads)+1;
        int x1=0;
        for(x1=0;x1<threads;x1++){
            // this part runs on parallel threads
            td[x1].tin=st;
            td[x1].tout=min(st+y,n);
		    td[x1].tjt = k+1;
            td[x1].tnt = n;
            td[x1].A=a;
            td[x1].P=p;
            td[x1].L=l;
            td[x1].U=u;

            int thread_error = pthread_create(&pthr[x1],NULL ,thread_routine, (void*) &td[x1]);
            if(thread_error){
                cout<<"Thread error\n";
            }
            
            st+=y;

            if(st>=n){
                break;
            }
        }
        

        for(int j=0;j<threads;++j){
            pthread_join(pthr[j], NULL);
        }

    }

    *error = 0;
    return;
}

// this function uses serial execution to compute LU decomposition using a single thread
void luserial(int n,double** a,double** p,double** l,double** u,int* error){

    for(int k=0;k<n;k++){
        double max = 0;
        int k_;        

        for(int i=k;i<n;i++){
            if(max < abs(a[i][k])){
                max=abs(a[i][k]);
                k_=i;
            }
        }

        if(max==0){
            *error = -1;
            return;
        }else{
            swap(a[k], a[k_]);
            swap(p[k], p[k_]);
            for(int i=0;i<k-1;i++){
                swap(l[k][i], l[k_][i]);
            }
        }

        u[k][k] = a[k][k];

        for (int i = k; i<n; i++){
            l[i][k] = a[i][k]/u[k][k];
            u[k][i] = a[k][i];
        }

        for(int i=k; i<n; i++){
            for(int j=k+1; j<n; j++){
                a[i][j] = a[i][j] - l[i][k]*u[k][j];
            }
        }
    }

    *error = 0;
    return;
}

// this function used omp to computer the LU decomposition
void luomp(int n,double** a,double** p,double** l,double** u,int* error,int threads){

    for(int k=0;k<n;k++){
        double max = 0;
        int k_;        

        
        #pragma omp parallel for shared(max,k_)
        for(int i=k;i<n;i++){
            if(max < abs(a[i][k])){
                max=abs(a[i][k]);
                k_=i;
            }
        }

        if(max==0){
            *error = -1;
        }else{
            swap(p[k], p[k_]);
            swap(a[k], a[k_]);
            #pragma omp parallel for num_threads(threads)
            for(int i=0;i<k-1;i++){
                swap(l[k][i], l[k_][i]);
            }
        }

        u[k][k] = a[k][k];

        #pragma omp parallel for num_threads(threads)
        for (int i = k; i<n; i++){
            l[i][k] = a[i][k]/u[k][k];
            u[k][i] = a[k][i];
        }

        // directive to parallelise both the inner loops
        #pragma omp parallel for collapse(2) num_threads(threads)
        for(int i=k; i<n; i++){
            for(int j=k+1; j<n; j++){
                a[i][j] = a[i][j] - l[i][k]*u[k][j];
            }
        }
    }

    *error = 0;
}

// this function takes matric P,A,L,U and computes the L1 norm of PA-LU
// this fuction is O(n^3)
double verification(int n,double** p,double** a,double** l,double** u){
    double error = 0;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int k=0;k<n;k++){
                error += p[i][k]*a[k][j] - l[i][k]*u[k][j];
            }
        }
    }
    return error;
}


// function to print the matrix supplied
void print_mat(double** m,int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%.2f ",m[i][j]);
        }
        cout << "\n";
    }
    cout << "\n";
}