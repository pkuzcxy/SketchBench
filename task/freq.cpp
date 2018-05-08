#include "../sketch/CmSketch.h"
#include "../sketch/CSketch.h"
#include "../sketch/CsmSketch.h"
#include "../sketch/CuSketch.h"

#include "../sketch/ASketch.h"
#include "../sketch/Count-Mean-MinSketch.h"
#include "../sketch/LossyCounting.h"
#include "../sketch/Spectral Bloom Filter.h"



#include "../dataset/StreamData.h"
#include "../hash/BOBHash.h"

#include <unordered_map>
#include <string>
#include <iostream>

using namespace std;

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

    unordered_map<string, int> s;
    char str[bytesPerStr];

    while (dat.GetNext(str)) {
        ++s[string(str, bytesPerStr)];
        cm.Insert(str, bytesPerStr);
        csm.Insert(str, bytesPerStr);
        cs.Insert(str, bytesPerStr);
        cu.Insert(str, bytesPerStr);
        as.Insert(str, bytesPerStr);
        cmm.Insert(str, bytesPerStr);
    }
    cout << "4\tcm\tcsm\tcs\tcu\ta\tcmm\tsbf" << endl;
    for (const auto& p: s) {
        cout << p.second << "\t";
        cout << cm.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << csm.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << cs.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << cu.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << as.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << cmm.Query(p.first.c_str(),bytesPerStr) << "\t";
        cout << endl;
    }
    return 0;
}
