#include <iostream>
#include <pthread.h>
#include <omp.h>
#include <chrono>

// define the thread_data struct to store the data for each thread run during pthreading
struct thread_data
{
    int tin;
    int tout;
    int tjt;
    int tnt;
    double **A;
    double **L;
    double **P;
    double **U;
};

// thread routine variable that runs on different threads
void* thread_routine(void* arg);
// this function uses pthreads to compute the LU decomposition using pthreads
void lupthreads(int n,double** a,double** p,double** l,double** u,int* error,int threads);
// this function uses serial execution to compute LU decomposition using a single thread
void luserial(int n,double** a,double** p,double** l,double** u,int* error);
// this function used omp to computer the LU decomposition
void luomp(int n,double** a,double** p,double** l,double** u,int* error,int threads);
// this function takes matric P,A,L,U and computes the L1 norm of PA-LU
double verification(int n,double** p,double** a,double** l,double** u);
// function to print the matrix supplied
void print_mat(double** m,int n);