#ifndef A_SKETCH_H
#define A_SKETCH_H


#define REDUCE (filterParameter+2*sizeof(Unit))*numStoredElephant_p/bit_per_counter/hash_num

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class ASketch: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::hash;
    using SketchBase<Hash, Unit>::data;
    int numStoredElephant;
    Unit *new_count;
    Unit *old_count;
    char **item;
public:
    using SketchBase<Hash, Unit>::sketch_name;
    ASketch(int hash_num, int bit_per_counter, int counter_per_array,int filterParameter,int numStoredElephant_p): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array-REDUCE)
    {
        numStoredElephant = numStoredElephant_p;
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
    int find_element_in_filter(const char *str)
    {
        for(int i=0;i<numStoredElephant;i++)
        {
            if(strcmp(str,item[i])==0)
                return  i;
        }
        return -1;
    }
    int find_empty_in_filter()
    {
        for(int i=0;i<numStoredElephant;i++)
        {
            if(strlen(item[i])==0)
                return i;
        }
        return -1;
    }
    void Insert(const char *str, const int len) {
        int index=find_element_in_filter(str);
        //元素在filter中
        if(index!=-1)
        {
            new_count[index]++;
            return;
        }
        //元素不在filter中，同时filter中还有空位
        int index_empty=find_empty_in_filter();
        if(index_empty!=-1)
        {
            strcpy(item[index_empty],str);
            new_count[index_empty]=1;
            old_count[index_empty]=0;
            return;
        }
        ++data[0][hash[0].Run(str, len) % counter_per_array];
        Unit estimate_freq=data[0][hash[0].Run(str, len) % counter_per_array];
        //元素不在filter中，同时filter中没有空位，在sketch中操作
        for (int i = 1; i < hash_num; ++i) {
            ++data[i][hash[i].Run(str, len) % counter_per_array];
            if(estimate_freq>data[i][hash[i].Run(str, len) % counter_per_array])
                estimate_freq=data[i][hash[i].Run(str, len) % counter_per_array];
        }
        int filter_min_index=0;//找到filter中最小的元组
        Unit filter_min_freq=new_count[0];
        for(int i=1;i<32;i++)
        {
            if(filter_min_freq>new_count[i])
            {
                filter_min_freq=new_count[i];
                filter_min_index=i;
            }
        }
        if(estimate_freq>filter_min_freq)
        {
            Unit diff=new_count[filter_min_index]-old_count[filter_min_index];
            if(diff>0)
            {
                for(int i=0;i<hash_num;++i)
                    data[i][hash[i].Run(str, len) % counter_per_array]+=diff;

            }
            strcpy(item[filter_min_index],str);
            new_count[filter_min_index]=estimate_freq;
            old_count[filter_min_index]=estimate_freq;
        }
    }
    Unit Query(const char *str, const int len) {
        int index = find_element_in_filter(str);
        if(index!=-1)
            return new_count[index];
        Unit res=data[0][hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            Unit t = data[i][hash[i].Run(str, len) % counter_per_array];
            res = res < t ? res : t;
        }
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
    }
};

#endif
