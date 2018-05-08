#ifndef CSM_SKETCH_H
#define CSM_SKETCH_H

#include "SketchBase.h"
#include <cstdlib>

// allows overflow
// Unit should not be unsigned
// result can be negative

template<class Hash, class Unit>
class CsmSketch: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::bit_per_counter;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::data;
    int acc = 0;
    Unit ave = 0;
public:
    using SketchBase<Hash, Unit>::sketch_name;
    CsmSketch(int hash_num, int bit_per_counter, int counter_per_array): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array)
    {
        strcpy(sketch_name,"csmsketch");
    }
    void Insert(const char *str, const int len) {
        int idx = rand() % hash_num;
        ++data[idx][hash[idx].Run(str, len) % counter_per_array];
        acc = (acc == counter_per_array - 1) ? 0 : acc + 1;
        ave += (acc == 0);
    }
    Unit Query(const char *str, const int len) {
        Unit res = -ave;
        for (int i = 0; i < hash_num; ++i) {
            res += data[i][hash[i].Run(str, len) % counter_per_array];
        }
        res = res>0?res:0;
        unsigned int upbound = (1<<bit_per_counter) -1;
        res = res>upbound ? upbound: res;
        return res;
    }
};

#endif