#ifndef A_SKETCH_H
#define A_SKETCH_H


#define REDUCE (filterParameter+2*sizeof(Unit))*numStoredElephant_p*8/bit_per_counter/hash_num

#include "SketchBase.h"
#include <string.h>
// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class ASketch: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::data;
    using SketchBase<Hash, Unit>::bit_per_counter;
    int numStoredElephant;
    int sz = 0;
    Unit *new_count;
    Unit *old_count;
    char **item;
    int filterParameter_p;
public:
    using SketchBase<Hash, Unit>::sketch_name;
    ASketch(int hash_num, int bit_per_counter, int counter_per_array,int filterParameter,int numStoredElephant_p): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array)
    {
        numStoredElephant = numStoredElephant_p;
        filterParameter_p = filterParameter;
        strcpy(sketch_name,"asketch");
        new_count=new Unit[numStoredElephant];
        old_count=new Unit[numStoredElephant];
        memset(new_count,0,sizeof(Unit)*numStoredElephant);
        memset(old_count,0,sizeof(Unit)*numStoredElephant);
        item = new char*[numStoredElephant];
        for(int i=0;i<numStoredElephant;i++)
        {
            item[i]=new char[filterParameter+1];
            item[i][0]='\0';
        }
    }
    int find_element_in_filter(const char* str, const int len)
    {
        for(int i=0;i<numStoredElephant;i++)
        {
            if(memcmp(str,item[i],len)==0)
                return  i;
        }
        return -1;
    }
    int minpos() {
        int res = 0, sm = new_count[0];
        for (int i = 1; i < sz; ++i) {
            if (new_count[i] < sm) {
                sm = new_count[i];
                res = i;
            }
        }
        return res;
    }
    void Insert(const char *str, const int len) {
        int p = find_element_in_filter(str, len);
        if (p != -1) {
            ++new_count[p];
            return;
        }
        if (sz<numStoredElephant)
        {
            memcpy(item[sz], str, len);
            new_count[sz] = 1;
            old_count[sz] = 0;
            ++sz;
            return;
        }
        for (int i = 0; i < hash_num; ++i) {
            ++data[i][hash[i].Run(str, len) % counter_per_array];
        }
        Unit res = data[0][hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            Unit t = data[i][hash[i].Run(str, len) % counter_per_array];
            res = res < t ? res : t;
        }
        unsigned int upbound = (1<<bit_per_counter) -1;
        res = res>upbound ? upbound: res;
     
        p = minpos();
        if (res > new_count[p])
        {
            if (new_count[p] > old_count[p])
            {
                for (int k = 0; k < new_count[p] - old_count[p]; ++k)
                {
                    for (int i = 0; i < hash_num; ++i)
                    {
                        ++data[i][hash[i].Run(str, len) % counter_per_array];
                    }
                }
            }
            memcpy(item[p], str, len);
            new_count[p] = res;
            old_count[p] = res;
        }
    }
    Unit Query(const char *str, const int len) {
        int index = find_element_in_filter(str,len);
        if(index!=-1)
            return new_count[index];
        Unit res=data[0][hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            Unit t = data[i][hash[i].Run(str, len) % counter_per_array];
            res = res < t ? res : t;
        }
        int upbound = (1<<bit_per_counter) -1;
        res = res>upbound ? upbound: res;
        return res;
    }
    ~ASketch()
    {
        
        delete new_count;
        delete old_count;
        for(int i=0;i<numStoredElephant;i++)
        {
            delete item[i];
        }
        delete [] item;
    }
    void clear()
    {
        delete new_count;
        delete old_count;
        for(int i=0;i<numStoredElephant;i++)
        {
            delete item[i];
        }
        delete [] item;
        new_count=new Unit[numStoredElephant];
        old_count=new Unit[numStoredElephant];
        memset(new_count,0,sizeof(Unit)*numStoredElephant);
        memset(old_count,0,sizeof(Unit)*numStoredElephant);
        item = new char*[numStoredElephant];
        for(int i=0;i<numStoredElephant;i++)
        {
            item[i]=new char[filterParameter_p+1];
            item[i][0]='\0';
        }
        sz = 0;
        for (int i = 0; i < hash_num; ++i) {
            for (int j = 0; j < counter_per_array; ++j) {
                data[i][j] = 0;
            }
        }
    }
};

#endif
