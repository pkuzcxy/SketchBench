#ifndef LCU_SKETCH_H
#define LCU_SKETCH_H

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class LossyCUSketch: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::bit_per_counter;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::data;
    using SketchBase<Hash, Unit>::MAX_HASH_NUM;
    int hashIdx[MAX_HASH_NUM];
    int cnt = 0, windowIdx = 1;
public:
    using SketchBase<Hash, Unit>::sketch_name;
    LossyCUSketch(int hash_num, int bit_per_counter, int counter_per_array): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array)
    {
        strcpy(sketch_name,"Lcusketch");
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
        ++cnt;
        if (cnt == counter_per_array * hash_num) {
            cnt = 0;
            for (int i = 0; i < hash_num; ++i) {
                for (int j = 0; j < counter_per_array; ++j) {
                    if (data[i][j] <= windowIdx && data[i][j] > 0) {
                        --data[i][j];
                    }
                }
            }
            ++windowIdx;
        }
    }
    Unit Query(const char *str, const int len) {
        Unit res = data[0][hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            Unit t = data[i][hash[i].Run(str, len) % counter_per_array];
            res = res < t ? res : t;
        }
        unsigned int upbound = (1<<bit_per_counter) -1;
        res = res>upbound ? upbound: res;
        return res;
    }
    void clear()
    {
        for (int i = 0; i < hash_num; ++i) {
            for (int j = 0; j < counter_per_array; ++j) {
                data[i][j] = 0;
            }
        }
    }
};

#endif