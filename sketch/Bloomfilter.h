#ifndef BloomFilter_H
#define BloomFilter_H

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class BloomFilter: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::bit_per_counter;
    using SketchBase<Hash, Unit>::data;
public:
    using SketchBase<Hash, Unit>::sketch_name;
    BloomFilter(int hash_num, int bit_per_counter, int counter_per_array): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array)
    {
        strcpy(sketch_name,"bloomfilter");
        hash[1].SetSeed(1 + 1000);
        hash[2].SetSeed(2 + 1000);
    }
    void Insert(const char *str, const int len)
    {
        for (int i = 0; i < 3; ++i)
            data[0][hash[i].Run(str, len) % counter_per_array]  = 1;
    }
    Unit Query(const char *str, const int len)
    {
        int cur = 0;
        for (int j = 0; j < 3; ++j)
            cur += data[0][hash[j].Run(str, len) % counter_per_array];
        if(cur ==3)
            return 1;
        else
            return 0;
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