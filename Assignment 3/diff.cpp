#include<bits/stdc++.h>

using namespace std;

int main(int argc,char** argv){
    if(argc!=3){
        return -1;
    }

    vector<float> file1;
    vector<float> file2;
    string line;

    ifstream input1(argv[1]);

    while(getline(input1,line)){
        stringstream ss(line);
        string s;
        float value;
        ss >> s;
        if(s!="s"){
            ss >> s;
            ss >> value;
            file1.push_back(value);
        }else{
            break;
        }
    }

    ifstream input2(argv[2]);
    
    while(getline(input2,line)){
        stringstream ss(line);
        string s;
        float value;
        ss >> s;
        if(s!="s"){
            ss >> s;
            ss >> value;
            file2.push_back(value);
        }else{
            break;
        }
    }

    double diff =0;
    for(int i=0;i<file1.size();i++){
        diff += fabs(file1[i]-file2[i]);
    }
    cout << "Absolute Difference: " << diff << endl;

}