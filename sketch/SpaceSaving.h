#include <string>
#include <vector>

class SpaceSaving {
private:
    std::vector<std::pair<std::string, int> > counter;
    const int m;
public:
    SpaceSaving(const int _m): m(_m) {}
    void Insert(const char* str, const int len) {
        const std::string s(str, len);
        for (int i = (int)counter.size() - 1; i >= 0; --i) {
            if (counter[i].first == s) {
                ++counter[i].second;
                return;
            }
        }
        if (counter.size() < m) {
            counter.push_back(std::make_pair(s, 1));
            return;
        }
        counter[m-1].first = s;
        counter[m-1].second += 1;
        for (int i = m-1; i > 0; --i) {
            if (counter[i].second > counter[i-1].second) {
                std::swap(counter[i], counter[i-1]);
            }
        }
    }
    std::vector<std::pair<std::string, int> > TopK(const int k) {
        return std::vector<std::pair<std::string, int> >(counter.begin(), counter.begin() + k);
    }
};
