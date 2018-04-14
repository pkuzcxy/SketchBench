#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[]) {
	
	if (argc != 3) {
        cout << "Should have two params!" << endl;
        return 1;
    }

    int n;
    ifstream in(argv[1]);
    FILE *fp = fopen(argv[2], "wb");
    while (in >> n) {
        fwrite(&n, sizeof(n), 1, fp);
    }
    fclose(fp);

    return 0;
}