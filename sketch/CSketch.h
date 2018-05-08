#ifndef C_SKETCH_H
#define C_SKETCH_H

#include "SketchBase.h"
#include <algorithm>
// this sketch's Unit should NOT be unsigned!

template<class Hash, class Unit>
class CSketch: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::data;
    using SketchBase<Hash, Unit>::MAX_HASH_NUM;
    using SketchBase<Hash, Unit>::bit_per_counter;
    Hash hash2[MAX_HASH_NUM];
    Unit queArr[MAX_HASH_NUM];
public:
    using SketchBase<Hash, Unit>::sketch_name;
    CSketch(int hash_num, int bit_per_counter, int counter_per_array): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array) {
        for (int i = 0; i < hash_num; ++i) {
            hash2[i].SetSeed(i + 1000 + hash_num);
        }
        strcpy(sketch_name,"csketch");
    }
    void Insert(const char *str, const int len) {
        for (int i = 0; i < hash_num; ++i) {
            data[i][hash[i].Run(str, len) % counter_per_array] += ((hash2[i].Run(str, len) & 1) ? 1 : -1);
        }
    }
    Unit Query(const char *str, const int len) {
        for (int i = 0; i < hash_num; ++i) {
            queArr[i] = data[i][hash[i].Run(str, len) % counter_per_array];
            queArr[i] = ((hash2[i].Run(str, len) & 1) ? queArr[i] : -queArr[i]);
        }
        std::sort(queArr, queArr + hash_num);
        int res =(queArr[hash_num>>1] + queArr[(hash_num-1)>>1]) >> 1;
        res = res>0?res:0;
        unsigned int upbound = (1<<bit_per_counter) -1;
        res = res>upbound ? upbound: res;
        return res;
    }
};

#endif