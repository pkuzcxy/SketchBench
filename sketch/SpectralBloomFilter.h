#ifndef SBF_SKETCH_H
#define SBF_SKETCH_H

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class SpectralBloomFilterSketch: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::bit_per_counter;
    using SketchBase<Hash, Unit>::data;
public:
    using SketchBase<Hash, Unit>::sketch_name;
    SpectralBloomFilterSketch(int hash_num, int bit_per_counter, int counter_per_array): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array*hash_num)
    {
        strcpy(sketch_name,"sbfsketch");
    }
    void Insert(const char *str, const int len) {
        for (int i = 0; i < hash_num; ++i) {
            ++data[0][hash[i].Run(str, len) % counter_per_array];
        }
        
    }
    Unit Query(const char *str, const int len) {
        
        Unit res = data[0][hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            int t = data[0][hash[i].Run(str, len) % counter_per_array];
            res = res < t ? res : t;
        }
        int upbound = (1<<bit_per_counter) -1;
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