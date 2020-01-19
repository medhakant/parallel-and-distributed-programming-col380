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

    clock_t begin, end;
	double time_spent;
	
    if(atoi(argv[3])==0)
    {
        begin = clock();
        // cout<<"hello";
        LUdecomposition(n, A, P, L, U);
        end = clock();
        time_spent = ((double)(end - begin)) / CLOCKS_PER_SEC;
        cout<<time_spent<<endl<<endl;
        outputMatrix(P);
        cout<<endl;
        outputMatrix(L);
        cout<<endl;
        outputMatrix(U);
    }
    else if(atoi(argv[3])==1)
    {
        begin = clock();
        LUopenMP(num_threads, n, A, P, L, U);
        end = clock();
        time_spent = ((double)(end - begin)) / CLOCKS_PER_SEC;
        cout<<time_spent<<endl<<endl;
        outputMatrix(P);
        cout<<endl;
        outputMatrix(L);
        cout<<endl;
        outputMatrix(U);
    }
    else
    {

    }
}