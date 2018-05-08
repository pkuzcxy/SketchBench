#include "../sketch/CmSketch.h"
#include "../sketch/CSketch.h"
#include "../sketch/CsmSketch.h"
#include "../sketch/CuSketch.h"
#include "../sketch/SketchBase.h"
#include "../sketch/ASketch.h"
#include "../sketch/Count-Mean-MinSketch.h"
#include "../dataset/StreamData.h"
#include "../hash/BOBHash.h"

#include <unordered_map>
#include <string>
#include <iostream>
#include <time.h>
#include <vector>

using namespace std;


double timing(vector<string> & v, SketchBase<BOBHash,int> &sketch,const int bytesPerStr)
{
    
    clock_t start,finish;
    start = clock();
    for(auto iter = v.begin();iter!=v.end();iter++)
        sketch.Insert(iter->c_str(), bytesPerStr);
    finish = clock();
    
    
    
    
    return double(finish-start)/CLOCKS_PER_SEC;;
}
int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        cout << "Please input filepath" << endl;
        return -1;
    }
    
    
    
    // sketches
    CmSketch<BOBHash, int> cm(4, 16, 65536);
    CsmSketch<BOBHash, int> csm(4, 16, 65536);
    CSketch<BOBHash, int> cs(4, 16, 65536);
    CuSketch<BOBHash, int> cu(4, 16, 65536);
    ASketch<BOBHash,int> as(4,16,65536);
    CountMeanMinSketch<BOBHash, int> cmm(4,16,65536);
    
    
    
    
    
    // Data Source
    const int bytesPerStr = 4;
    StreamData dat(argv[1], bytesPerStr);
    
    
    char str[bytesPerStr];
    vector<string> v;
    while (dat.GetNext(str))
        v.push_back(string(str, bytesPerStr));
    

    cout << "TotalNum\tcm\tcsm\tcs\tcu\ta\tcmm" << endl;
    cout << v.size() << "\t";
    cout << timing(v,cm,bytesPerStr) << "\t";
    cout << timing(v,csm,bytesPerStr) << "\t";
    cout << timing(v,cs,bytesPerStr) << "\t";
    cout << timing(v,cu,bytesPerStr) << "\t";
    cout << timing(v,as,bytesPerStr) << "\t";
    cout << timing(v,cmm,bytesPerStr) << "\t";
    cout << endl;
    return 0;
}
