#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Record {
public:
    int ae, acc;
    double re;
    Record(const int real, const int my) {
        ae = real - my;
        ae = ae < 0 ? -ae : ae;
        re = ae / (double)real;
        acc = real == my;
    }
};

int main() {
    int n;
    cin >> n;
    vector<string> name(n, "");
    for (int i = 0; i < n; ++i) {
        cin >> name[i];
    }
    vector<vector<Record> > rec;
    int real, my;
    while (cin >> real) {
        rec.push_back(vector<Record>());
        for (int i = 0; i < n; ++i) {
            cin >> my;
            rec.back().push_back(Record(real, my));
        }
    }
    for (int i = 0; i < n; ++i) {
        cout << name[i] << ":" << endl;
        int ae = 0, acc = 0;
        double re = 0;
        int cdf[10][2];
        for (int j = 0; j < 20; ++j) {
            cdf[j/2][j%2] = 0;
        }
        for (const auto& recc: rec) {
            ae += recc[i].ae;
            acc += recc[i].acc;
            re += recc[i].re;
            for (int j = 0; j < 10; ++j) {
                if (recc[i].ae <= j) ++cdf[j][0];
                if (recc[i].re <= (double)j / 10) ++cdf[j][1];
            }
        }
        cout << "aae: " << (double)ae / rec.size() << endl;
        cout << "are: " << (double)re / rec.size() << endl;
        cout << "acc: " << (double)acc / rec.size() << endl;
        cout << "ae-cdf:";
        for (int j = 0; j < 10; ++j) cout << " " << (double)cdf[j][0] / rec.size();
        cout << endl;
        cout << "re-cdf:";
        for (int j = 0; j < 10; ++j) cout << " " << (double)cdf[j][1] / rec.size();
        cout << endl;
    }
}