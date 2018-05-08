#ifndef CU_SKETCH_H
#define CU_SKETCH_H

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class CuSketch: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::data;
    using SketchBase<Hash, Unit>::MAX_HASH_NUM;
    int hashIdx[MAX_HASH_NUM];
public:
    using SketchBase<Hash, Unit>::sketch_name;
    CuSketch(int hash_num, int bit_per_counter, int counter_per_array): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array)
    {
        strcpy(sketch_name,"cusketch");
    }
    void Insert(const char *str, const int len) {
        hashIdx[0] = hash[0].Run(str, len) % counter_per_array;
        Unit sm = data[0][hashIdx[0]];
        for (int i = 1; i < hash_num; ++i) {
            hashIdx[i] = hash[i].Run(str, len) % counter_per_array;
            Unit t = data[i][hashIdx[i]];
            sm = sm < t ? sm : t;
        }
        for (int i = 0; i < hash_num; ++i) {
            data[i][hashIdx[i]] += (data[i][hashIdx[i]] == sm);
        }
    }
    Unit Query(const char *str, const int len) {
        Unit res = data[0][hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            Unit t = data[i][hash[i].Run(str, len) % counter_per_array];
            res = res < t ? res : t;
        }
        return res;
    }
};

#endif