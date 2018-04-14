#include "../sketch/CmSketch.h"
#include "../sketch/CSketch.h"
#include "../sketch/CsmSketch.h"
#include "../sketch/CuSketch.h"
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
    CmSketch<BOBHash, int> cm(4, 16, 1000);
    CsmSketch<BOBHash, int> csm(4, 16, 1000);
    CSketch<BOBHash, int> cs(4, 16, 1000);
    CuSketch<BOBHash, int> cu(4, 16, 1000);

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
    }
    cout << "4\tcm\tcsm\tcs\tcu" << endl;
    for (const auto& p: s) {
        cout << p.second << "\t";
        cout << cm.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << csm.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << cs.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << cu.Query(p.first.c_str(), bytesPerStr) << "\t";
        cout << endl;
    }
    return 0;
}