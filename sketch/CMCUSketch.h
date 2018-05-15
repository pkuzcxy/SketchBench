#ifndef CMCU_SKETCH_H
#define CMCU_SKETCH_H

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class CMCUSketch: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::bit_per_counter;
    using SketchBase<Hash, Unit>::data;
public:
    using SketchBase<Hash, Unit>::sketch_name;
    CMCUSketch(int hash_num, int bit_per_counter, int counter_per_array): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array)
    {
        strcpy(sketch_name,"CMCUSketch");
    }
    void Insert(const char *str, const int len) {
        Unit temp=data[0][hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            if(data[i][hash[i].Run(str, len) % counter_per_array]<temp)
                temp=data[i][hash[i].Run(str, len) % counter_per_array];
        }
        for(int i=0;i<hash_num;++i){
            if(data[i][hash[i].Run(str, len) % counter_per_array]==temp)
                ++data[i][hash[i].Run(str, len) % counter_per_array];
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
};

#endif
