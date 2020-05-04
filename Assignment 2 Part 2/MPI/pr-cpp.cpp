#include<vector>
#include<iostream>
#include<sstream>
#include<fstream>
#include<cmath>
#include<chrono>

using namespace std;
using namespace std::chrono;

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

vector<float> pagerankgenerator(vector<vector<int>> incominglinks,vector<int> numlinks,vector<int> leafnodes,double alpha,double convergence,int checksteps){
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

        for(int j=0;j<n;j++){
            vector<int> page = incominglinks[j];
            double dot = 0;
            for(int k=0;k<page.size();k++){
                dot += iold[page[k]]/numlinks[page[k]];
            }
            inew[j] = dot*alpha + oneav + oneiv;
        }

        diff = 0;
        for(int i=0;i<n;i++){
            diff += fabs(inew[i]-iold[i]);
        }
        i++;
    }
    return inew;
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

vector<float> pagerank(vector<vector<int>> linkmatrix,double alpha,double convergence,int checksteps){
    vector<vector<int>> incominglinks;
    vector<int> numlinks;
    vector<int> leafnodes;
    transposelinkmatrix(linkmatrix,incominglinks,numlinks,leafnodes);
    vector<float> gr = pagerankgenerator(incominglinks,numlinks,leafnodes,alpha,convergence,checksteps);
    return gr;
}


int main(int argc,char** argv){
    string inputFilename = argv[1];
    vector<vector<int>> links;
    readfile(inputFilename,links);
    time_point<system_clock> start, end;
    duration<double> elapsed_seconds;
    start = system_clock::now(); 
    vector<float> pr = pagerank(links,0.85,0.00001,100000);
    end = system_clock::now(); 
    elapsed_seconds = end - start; 

    // print time elapsed in the computation
    cout<<"Execution Time: "<<elapsed_seconds.count()<<endl;
    double sum = 0;

    if(argc==4){
        string outputFilename = argv[3];
        ofstream outputFile(outputFilename);
        for(int i=0;i<pr.size();i++){
            outputFile << i << " = " << pr[i] << endl;
            sum += pr[i];
        }
        outputFile << "s = " << sum << endl;
        outputFile.close();

    }else{
        for(int i=0;i<pr.size();i++){
            cout << i << " = " << pr[i] << endl;
            sum += pr[i];
        }
        cout << "s = " << sum << endl;
    }
}