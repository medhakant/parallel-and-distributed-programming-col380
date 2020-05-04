#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "mapreduce.h"
#include "keyvalue.h"
#include <math.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <numeric>

using namespace std;
using namespace std::chrono;
using namespace MAPREDUCE_NS;

vector<vector<int>> incominglinks;
vector<double> pgr_intermediate;
vector<float> pgr;

void mapper(int itask, KeyValue *kv, void *ptr){
    int world_rank, num_procs;
    MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int n = incominglinks.size();
    for (int i=((world_rank*n)/num_procs);i<(((world_rank+1)*n)/num_procs);i++) {
        double value = 0;
        vector<int> page = incominglinks[i];
        kv->add((char *) &i,sizeof(int),(char *) &value,sizeof(double));
        for(int j =0; j<page.size(); j++){
            value = pgr_intermediate[page[j]];
            kv->add((char *)&i,sizeof(int),(char *) &value,sizeof(double));
        }
    }
}

void reducer(char *key, int keybytes, char *multivalue,int nvalues, int *valuebytes, KeyValue *kv, void *ptr){
  int t = *(int *) key;
  double* s = (double *) multivalue;
  double sum = 0;
  for(int i = 0; i < nvalues; i++){
    sum += s[i];
  }
  kv->add(key,keybytes,(char *) &sum,sizeof(double));
}

void update(uint64_t itask, char *key, int keybytes, char *value,int valuebytes, KeyValue *kv, void *ptr){
  int key_ = *(int *) key;
  double value_ = *(double *) value;
  pgr_intermediate[key_] = value_;
}

void readfile(string filename,vector<vector<int>>& links){
    ifstream inputFile(filename);
    string line;
    while(getline(inputFile,line)){
        stringstream ss(line);
        int from,to;
        ss >> from >> to;
        int extend = ((from > to)? from : to) - links.size() + 1;
        for(int i=0;i<extend;i++){
            vector<int> link;
            links.push_back(link);
        }
        links[from].push_back(to);
    }
    inputFile.close();
}

void pagerankgenerator(vector<vector<int>> incominglinks,vector<int> numlinks,vector<int> leafnodes,double alpha,double convergence,int checksteps){
    
}


void transposelinkmatrix(vector<vector<int>> outgoinglinks,vector<vector<int>>& incominglinks,vector<int>& numlinks,vector<int>& leafnodes){
    int npages = outgoinglinks.size();
    for(int i=0;i<npages;i++){
        vector<int> links;
        incominglinks.push_back(links);
        numlinks.push_back(0);
    }

    for(int i=0;i<npages;i++){
        if(outgoinglinks[i].size()==0){
            leafnodes.push_back(i);
        }else{
            numlinks[i] = outgoinglinks[i].size();
            for(int j=0;j<outgoinglinks[i].size();j++){
                int value = outgoinglinks[i][j];
                incominglinks[value].push_back(i);
            }
        }
    }    
}

void pagerank(int argc,char** argv){
    MPI_Init(&argc,&argv);
    int rank,nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

    string inputFilename = argv[1];
    vector<vector<int>> linkmatrix;
    vector<int> numlinks;
    vector<int> leafnodes;
    double start, end;
    double elapsed_seconds;
    double convergence = 0.0001;
    double alpha = 0.85;
    int checksteps = 10000;
    readfile(inputFilename,linkmatrix);
    transposelinkmatrix(linkmatrix,incominglinks,numlinks,leafnodes);

    start = MPI_Wtime();
    MapReduce *mr;
    int n = incominglinks.size();
    int m = leafnodes.size();
    vector<float> inew(n,1.0/n),iold(n,1.0/n);
    bool done = false;
    double diff = 1.0f;
    int i=0;
    while(diff > convergence && i < checksteps){
        double sum = 0;

        for(int i=0;i<n;i++){
            sum += inew[i];
        }
        
        for(int i=0;i<n;i++){
            inew[i] = inew[i]/sum;
        }

        swap(inew,iold);

        double sum_iold = 0;

        for(int j=0;j<n;j++){
            sum_iold += iold[j];
        }

        double oneiv = (1-alpha)*sum_iold/n;

        double oneav = 0;

        double sum_iold_take = 0;
        for(int j=0;j<m;j++){
            sum_iold_take += iold[leafnodes[j]];
        }
        oneav = alpha*sum_iold_take/n;

        pgr_intermediate.clear();

        for(int j=0;j<n;j++){
            if(numlinks[j]>0){
                pgr_intermediate.push_back(iold[j]*1.0/numlinks[j]);
            }else{
                pgr_intermediate.push_back(0);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        mr = new MapReduce(MPI_COMM_WORLD);
        int nwords = mr->map(nprocs,mapper,NULL);
        int nfiles = mr->mapfilecount;
        mr->gather(1);
        mr->convert();
        int nunique = mr->reduce(reducer,NULL);
        mr->broadcast(0);
        MPI_Barrier(MPI_COMM_WORLD);
        mr->map(mr,update,NULL);
        delete mr;

        for (int k=0;k<n;k++){
            inew[k] = alpha*pgr_intermediate[k] + oneav + oneiv;
        }

        diff = 0;
        for(int i=0;i<n;i++){
            diff += fabs(inew[i]-iold[i]);
        }
        i++;
    }
    pgr = inew;
    end = MPI_Wtime();
    elapsed_seconds = end - start; 

    if(rank==0){
        // print time elapsed in the computation
        cout<<"Execution Time: "<<elapsed_seconds<<endl;
        double sum = 0;
        if(argc==4){
            string outputFilename = argv[3];
            ofstream outputFile(outputFilename);
            for(int i=0;i<pgr.size();i++){
                outputFile << i << " = " << pgr[i] << endl;
                sum += pgr[i];
            }
            outputFile << "s = " << sum << endl;
            outputFile.close();

        }else{
            for(int i=0;i<pgr.size();i++){
                cout << i << " = " << pgr[i] << endl;
                sum += pgr[i];
            }
            cout << "s = " << sum << endl;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}


int main(int argc,char** argv){
    pagerank(argc,argv);
}