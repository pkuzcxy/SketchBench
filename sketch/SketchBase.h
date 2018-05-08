#ifndef SKETCH_BASE_H
#define SKETCH_BASE_H

#include <cstdio>
#include <cstring>
// bit_per_counter is ONLY for error checking

template<class Hash, class Unit>
class SketchBase {
protected:
    static const int MAX_HASH_NUM = 20;
    Unit *data[20];
    Hash hash[20];
    bool checkParamLegal() {
        if ((sizeof(Unit) << 3) < bit_per_counter) {
            printf("Unit smaller than %d bits.\n", bit_per_counter);
            return false;
        }
        if (hash_num > MAX_HASH_NUM || hash_num <= 0) {
            printf("Invalid hash num %d\n", hash_num);
            return false;
        }
        return true;
    }
    void allocMem() {
        for (int i = 0; i < hash_num; ++i) {
            data[i] = new Unit [counter_per_array];
            for (int j = 0; j < counter_per_array; ++j) {
                data[i][j] = 0;
            }
        }
    }
    void initHash() {
        for (int i = 0; i < hash_num; ++i) {
            hash[i].SetSeed(i + 1000);
        }
    }
public:
    const int hash_num, bit_per_counter, counter_per_array;
    char sketch_name[50];
    int MemUsageInBit() {
        return hash_num * bit_per_counter * counter_per_array;
    }
    SketchBase(int _hash_num, int _bit_per_counter, int _counter_per_array): hash_num(_hash_num), bit_per_counter(_bit_per_counter), counter_per_array(_counter_per_array) {
        if (checkParamLegal()) {
            allocMem();
            initHash();
        }
    }
    ~SketchBase() {
        for (int i = 0; i < hash_num; ++i) {
            delete [] data[i];
        }
    }
 
    
    virtual void Insert(const char *str, int len) = 0;
    virtual Unit Query(const char *str, const int len) = 0;
};

#endif