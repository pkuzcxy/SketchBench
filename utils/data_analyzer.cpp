#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include "../dataset/StreamData.h"
using namespace std;

int main(int argc, char *argv[]) {

    if (argc != 3) {
        cout << "Param[1] bytesPerStr, Param[2] filepath" << endl;
        return 1;
    }

    const int len = atoi(argv[1]);
    StreamData sd(argv[2], len);
	unordered_map<string, int> s;
    char str[len];
    int sm = 2147483647, bg = 0, tot = 0;

    while (sd.GetNext(str)) {
    	int t = ++s[string(str, len)];
    	sm = sm > t ? t : sm;
    	bg = bg < t ? t : bg;
    	++tot;
    }

    cout << "Unique: " << s.size() << endl;
    cout << "Small: " << sm << endl;
    cout << "Big: " << bg << endl;
    cout << "Total: " << tot << endl;
}