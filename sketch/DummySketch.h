#ifndef DUMMYSKETCH_H
#define DUMMYSKETCH_H

#include <unordered_map>
#include <string>
#include "BaseSketch.h"

class DummySketch: public BaseSketch<int, void> {
private:
    std::unordered_map<std::string, int> s;
public:
    bool init();
    void insert(const char* str, const unsigned int len);
    int query(const char* str, const unsigned int len);
};

// implementation

bool DummySketch::init() {}
void DummySketch::insert(const char* str, const unsigned int len) {
    ++s[std::string(str, len)];
}
int DummySketch::query(const char* str, const unsigned int len) {
    return s[std::string(str, len)]; 
}

#endif