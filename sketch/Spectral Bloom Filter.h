#ifndef CM_SKETCH_H
#define CM_SKETCH_H

#include "BloomFilterBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class SpectralBloomFilter: public BloomFilterBase<Hash, Unit> {
private:
    using BloomFilterBase<Hash, Unit>::hash_num;
    using BloomFilterBase<Hash, Unit>::counter_of_array;
    using BloomFilterBase<Hash, Unit>::hash;
    using BloomFilterBase<Hash, Unit>::data;
public:
    SpectralBloomFilter(int hash_num, int bit_per_counter, int counter_of_array): BloomFilterBase<Hash, Unit>(hash_num, bit_per_counter, counter_of_array){}
    void Insert(const char *str, const int len) {
        for (int i = 0; i < hash_num; ++i) {
            ++data[hash[i].Run(str, len) % counter_per_array];
        }
    }
    Unit Query(const char *str, const int len) {
        Unit ans,temp;
        ans = data[hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            temp = data[hash[i].Run(str, len) % counter_per_array];
            if(ans>temp)
                ans=temp;
        }
        return ans;
    }
};

#endif
