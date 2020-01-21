#include "library.h"
#include "defs.h"
#include "functions.h"
// #include <bits/stdc++.h> 

vi zero(int n)
{
    vi vec;
    for(int i=0;i<n;i++)
        vec.pb(0.0);
    return vec;
}

void createMatrix(vvi &mat, int n)
{
    for(int i=0;i<n;i++)
        mat.pb(zero(n));
}

void inputMatrix(vvi &A, int n)
{
    for(int i=0;i<n;i++)
    {
        vi temp;
        for(int j=0;j<n;j++)
        {
            double x;
            cin>>x;
            temp.pb(x);
        }
        
        A.pb(temp);
    }
}

void outputMatrix(vvi &mat)
{
    int n=mat.size();
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            cout<<mat[i][j]<<" ";
        }
        cout<<endl;
    }
}

int main(int argc, char** argv)
{
    cout<<fixed<<setprecision(5)<<endl;
    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    // cout<<n<<endl;
    vvi A, P, L, U;

    inputMatrix(A, n);
    createMatrix(P, n);
    createMatrix(L, n);
    createMatrix(U, n); 

    for(int i=0;i<n;i++)
        L[i][i]=1;

    std::chrono::time_point<std::chrono::system_clock> start, end;
    // start = std::chrono::system_clock::now(); 
    // cout<<argv[3]<<endl;
	
    if(atoi(argv[3])==0)
    {
        start = std::chrono::system_clock::now(); 
        // cout<<"hello";
        LUdecomposition(n, A, P, L, U);
        end = std::chrono::system_clock::now(); 
        std::chrono::duration<double> elapsed_seconds = end - start; 
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        cout<<elapsed_seconds.count()<<endl<<endl;
        /* outputMatrix(P);
        cout<<endl;
        outputMatrix(L);
        cout<<endl;
        outputMatrix(U); */
    }
    else if(atoi(argv[3])==1)
    {
        start = std::chrono::system_clock::now(); 
        // cout<<"hello"<<endl;
        LUopenMP(num_threads, n, A, P, L, U);
        end = std::chrono::system_clock::now(); 
        std::chrono::duration<double> elapsed_seconds = end - start; 
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        cout<<elapsed_seconds.count()<<endl<<endl;
        /* outputMatrix(P);
        cout<<endl;
        outputMatrix(L);
        cout<<endl;
        outputMatrix(U); */
    }
    else
    {
        start = std::chrono::system_clock::now(); 
        LUpthreads(num_threads, n, A, P, L, U);
        end = std::chrono::system_clock::now(); 
        std::chrono::duration<double> elapsed_seconds = end - start; 
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        cout<<elapsed_seconds.count()<<endl<<endl;
    }
}