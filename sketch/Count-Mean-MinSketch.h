#ifndef CMM_SKETCH_H
#define CMM_SKETCH_H

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class CountMeanMinSketch: public SketchBase<Hash, Unit>
{
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::data;
    using SketchBase<Hash, Unit>::bit_per_counter;
    long long num_element;
public:
    using SketchBase<Hash, Unit>::sketch_name;
    CountMeanMinSketch(int hash_num, int bit_per_counter, int counter_per_array): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array)
    {
        strcpy(sketch_name,"cmmsketch");
        num_element=0;
    }
    void Insert(const char *str, const int len) {
        for (int i = 0; i < hash_num; ++i) {
            ++data[i][hash[i].Run(str, len) % counter_per_array];
        }
        num_element++;
    }
    Unit Query(const char *str, const int len) {
        Unit temp,noise;
        temp = data[0][hash[0].Run(str, len) % counter_per_array];
        noise=(num_element-temp)/(counter_per_array);
        Unit estimate=temp-noise;
        for (int i = 1; i < hash_num; ++i) {
            temp = data[i][hash[i].Run(str, len) % counter_per_array];
            noise=(num_element-temp)/(counter_per_array);
            estimate+=temp-noise;
        }
        
        int res = estimate/hash_num > 0 ?estimate/hash_num :0;
        unsigned int upbound = (1<<bit_per_counter) -1;
        res = res>upbound ? upbound: res;
        return res;
    }
};
#endif
