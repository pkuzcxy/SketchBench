#ifndef COUNTMINSKETCH_H
#define COUNTMINSKETCH_H

// allows overflow
#include <cstdio>
#include "BaseSketch.h"

template<class Hash, class Config, class Base>
class CountMinSketch: public BaseSketch<Base, Config> {
private:
    Config config;
    Base* data[20];
    Hash hash[20];
public:
    bool init(const Config& _config);
    void insert(const char* str, const unsigned int len);
    Base query(const char* str, const unsigned int len);
    ~CountMinSketch();
};

// implementation

template<class Hash, class Config, class Base>
bool CountMinSketch<Hash, Config, Base>::init(const Config& _config) {
    config = _config;
    if ((sizeof(Base) << 3) < config.bit_per_bucket) {
        printf("Base too small for bit per bucket\n");
        return false;
    }
    if (config.hash_num > 20) {
        printf("hash num too large\n");
        return false;
    }
    for (int i = 0; i < config.hash_num; ++i)
        hash[i].setSeed(i + 1000);
    for (int i = 0; i < config.hash_num; ++i)
        data[i] = new Base [config.bucket_per_array];
    return true;
}

template<class Hash, class Config, class Base>
CountMinSketch<Hash, Config, Base>::~CountMinSketch() {
    for (int i = 0; i < config.hash_num; ++i)
        delete [] data[i];
}

template<class Hash, class Config, class Base>
void CountMinSketch<Hash, Config, Base>::insert(const char* str, const unsigned int len) {
    for (int i = 0; i < config.hash_num; ++i) {
        ++data[i][hash[i].run(str, len)%config.bucket_per_array];
    }
}

template<class Hash, class Config, class Base>
Base CountMinSketch<Hash, Config, Base>::query(const char* str, const unsigned int len) {
    Base res = data[0][hash[0].run(str, len)%config.bucket_per_array];;
    for (int i = 1; i < config.hash_num; ++i) {
        Base t = data[i][hash[i].run(str, len)%config.bucket_per_array];
        res = res < t ? res : t;
    }
    return res;
}

#endif