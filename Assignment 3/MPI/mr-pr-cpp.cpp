#include<vector>
#include<iostream>
#include<sstream>
#include<fstream>
#include<cmath>
#include<chrono>
// #include"mapreduce.h"

using namespace std;
using namespace std::chrono;

const float ALPHA = 0.85;
const float CONVERGENCE = 0.00001;

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

void mapper_sum(int key, float pg,vector<vector<int>>& outgoing_links,vector<float>& sum){
	int sizes = outgoing_links[key].size();
	if(sizes==0){
		sum.push_back(pg);
	}
}

void mapper(int key, float pg,vector<vector<int>>& outgoing_links,vector<vector<float>>& pgr){
	pgr[key].push_back(0.0f);
	int sizes = outgoing_links[key].size();
	if(sizes!=0){
		for(int i =0; i<sizes; i++){
			pgr[outgoing_links[key][i]].push_back((float)(pg/sizes));
		}
	}
}

void reducer(int key, vector<float>& pg,vector<float>& pgr_new){
	float temp = 0.0f;
	for(int i =0; i<pg.size(); i++){
		temp += (float)pg[i];
	}
	pgr_new[key] = temp;
}

vector<float> pagerank(vector<vector<int>> linkmatrix){
    int n = linkmatrix.size();
    vector<float> pgr(n,1.0/n);
    vector<float> pgr_new(n);
    vector<vector<float>> pgr_i(n);
    vector<vector<int>> outgoing_links(n);
    vector<float> sum;
    while(true){
		for(int i=0; i<n; i++){
			mapper_sum(i,pgr[i],outgoing_links,sum);
		}
		float dp =0.0f;
		for(int i=0; i<sum.size(); i++){
			dp += sum[i];
		}
		dp = float(dp/n);
		sum.clear();
		for(int i=0; i<n; i++){
			mapper(i,pgr[i],outgoing_links,pgr_i);
		}
		for(int i=0; i<n; i++){
			reducer(i,pgr_i[i],pgr_new);
			pgr_i[i].clear();
		}
		for(int i=0; i<n; i++){
			pgr_new[i] = (float)ALPHA*pgr_new[i] + ALPHA*dp + (float)(1-ALPHA)/n;
		}
		bool converging = true;
		for(int i=0; i<n; i++){
			if(pgr[i]-pgr_new[i]>CONVERGENCE)
				converging=false;
			pgr[i] = pgr_new[i];
		}
		if(converging)
			break;
	}
    return pgr;
}


int main(int argc,char** argv){
    string inputFilename = argv[1];
    vector<vector<int>> links;
    readfile(inputFilename,links);

    time_point<system_clock> start, end;
    duration<float> elapsed_seconds;
    start = system_clock::now(); 

    vector<float> pgr = pagerank(links);

    end = system_clock::now(); 
    elapsed_seconds = end - start; 

    // print time elapsed in the computation
    cout<<"Execution Time: "<<elapsed_seconds.count()<<endl;
    float sum = 0;

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