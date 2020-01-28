// import the LU decomposition library
#include"ludecomp.h"

using namespace std;
using namespace std::chrono;

int main(int argc,char** argv){
    // if the number of arguments provided is not sufficient, print a message and exit
    if(argc != 4){
        cout << "Usage:-  ludecomp [mode] [n] [thread]\n";
        return -1;
    }
    
    // define variables to measure the time taken by the function
    time_point<system_clock> start, end;
    duration<double> elapsed_seconds;
    // mode variable stores the mode we are trying to run the program in
    int mode = atoi(argv[1]);
    int n= atoi(argv[2]);
    int threads = 4;

    if(argc==4){
        threads = atoi(argv[3]);
    }
    int error;

    // define matrices as pointer to pointers
    double** a;
    double** l;
    double** u;
    double** p;
    // make a copy of the matrix a as it might change during function call
    double** a_copy;

    // initialise the matrices
    p = new double*[n];
    a = new double*[n];
    l = new double*[n];
    u = new double*[n];
    a_copy = new double*[n];

    for(int i=0;i<n;i++){ 
        a[i]=new double[n];
        l[i]=new double[n];
        u[i]=new double[n];
        p[i]=new double[n];
        a_copy[i] = new double[n];
    }

    // fill the matrix a with random data and fill matrix l,u & p appropriately
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            a[i][j]=drand48()+0.2;
            a_copy[i][j] = a[i][j];
            if(i==j){
                l[i][j]=1;
                u[i][j]=1;
                p[i][j]=1;
            }else{
                l[i][j]=0;
                u[i][j]=0;
                p[i][j]=0;
            }
        }
    }

    start = system_clock::now(); 
    // print_mat(a,n);

    // call the appropriate function according to the input provided by the user
    if(mode==0){
        luomp(n,a,p,l,u,&error,threads);
    }else if(mode==1){
        lupthreads(n,a,p,l,u,&error,threads);
    }else{
        luserial(n,a,p,l,u,&error);
    }
    // print_mat(p,n);
    // print_mat(l,n);
    // print_mat(u,n);

    // print if an error  occured during execution. 0 is for no error.
    cout << "Error Y/N: " << error << endl;
    end = system_clock::now(); 
    elapsed_seconds = end - start; 

    // print time elapsed in the computation
    cout<<"Execution Time: "<<elapsed_seconds.count()<<endl;
    // cout<<"Verification PA-LU: " << verification(n,p,a_copy,l,u)<<endl;
}