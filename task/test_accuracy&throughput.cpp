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
#include <fstream>
#include <time.h>
#include <vector>

using namespace std;

string dataset;
void test(vector<string> & v,unordered_map<string, int> & item2freq, SketchBase<BOBHash,int> &sketch,const int bytesPerStr)
{
    
    string sketch_name = sketch.sketch_name;
    /*throuput test*/
    clock_t start,finish;
    start = clock();
    for(auto iter = v.begin();iter!=v.end();iter++)
        sketch.Insert(iter->c_str(), bytesPerStr);
    finish = clock();
    ofstream throughput_file;
    string throughput_file_name = "throughput_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitpercounter"+to_string(sketch.bit_per_counter)+\
    "_counterperarray"+to_string(sketch.counter_per_array);
    
    throughput_file.open(throughput_file_name);
    throughput_file << "TotalNum\tTime" << endl;
    throughput_file << v.size() << "\t";
    throughput_file << double(finish-start)/CLOCKS_PER_SEC << "\t";
    throughput_file.close();

    /*accuracy test*/
    ofstream frequency_file;
    string frequency_file_name =  "frequency_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitPerCounter"+to_string(sketch.bit_per_counter)+\
    "_counterPerArray"+to_string(sketch.counter_per_array);
    frequency_file.open(frequency_file_name);
    frequency_file<<"exact\testimated"<<endl;
    for (const auto& p: item2freq) {
        frequency_file << p.second << "\t";
        frequency_file << sketch.Query(p.first.c_str(), bytesPerStr) << "\t";
        frequency_file << endl;
    }
    frequency_file.close();
}
int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        cout << "Please input filepath" << endl;
        return -1;
    }
    // Data Source
    dataset = argv[1];
    const int bytesPerStr = 4;
    StreamData dat(argv[1], bytesPerStr);
    unordered_map<string, int> item2freq;
    char str[bytesPerStr];
    vector<string> v;
    while (dat.GetNext(str))
    {
        v.push_back(string(str, bytesPerStr));
        ++item2freq[string(str, bytesPerStr)];
    }

    // sketches
    CmSketch<BOBHash, int> cm(4, 16, 65536);
    CsmSketch<BOBHash, int> csm(4, 16, 65536);
    CSketch<BOBHash, int> cs(4, 16, 65536);
    CuSketch<BOBHash, int> cu(4, 16, 65536);

    /*ASketch_parameter: elephant item size,max num of items in filter */
    ASketch<BOBHash,int> as(4,16,65536,bytesPerStr,128);
    CountMeanMinSketch<BOBHash, int> cmm(4,16,65536);
    
    
    //test
    test(v,item2freq,cm,bytesPerStr);
    test(v,item2freq,csm,bytesPerStr);
    test(v,item2freq,cs,bytesPerStr);
    test(v,item2freq,cu,bytesPerStr);
    test(v,item2freq,as,bytesPerStr);
    test(v,item2freq,cmm,bytesPerStr);
    
    
    return 0;
}
