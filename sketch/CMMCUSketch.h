#ifndef CMMCU_SKETCH_H
#define CMMCU_SKETCH_H

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class CMMCUSketch : public SketchBase<Hash, Unit> {
private:
	using SketchBase<Hash, Unit>::hash_num;
	using SketchBase<Hash, Unit>::counter_per_array;
    using SketchBase<Hash, Unit>::bit_per_counter;
	using SketchBase<Hash, Unit>::hash;
	using SketchBase<Hash, Unit>::data;
	Unit *num_element;
    Unit t[10];
public:
    using SketchBase<Hash, Unit>::sketch_name;
	CMMCUSketch(int hash_num, int bit_per_counter, int counter_per_array) : SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array) {
		num_element = new int[hash_num];
		memset(num_element, 0, sizeof(int)*hash_num);
        strcpy(sketch_name,"cmmcusketch");
	}
	~CMMCUSketch() {
		delete [] num_element;
	}
	void Insert(const char *str, const int len) {
		Unit temp = data[0][hash[0].Run(str, len) % counter_per_array];
		for (int i = 1; i < hash_num; ++i) {
			if (data[i][hash[i].Run(str, len) % counter_per_array]<temp)
				temp = data[i][hash[i].Run(str, len) % counter_per_array];
		}
		for (int i = 0;i<hash_num;++i) {
			if (data[i][hash[i].Run(str, len) % counter_per_array] == temp)
			{
				++data[i][hash[i].Run(str, len) % counter_per_array];
				num_element[i]++;
			}
		}
	}
    Unit Query(const char *str, const int len) {
        Unit temp,noise;
        temp = data[0][hash[0].Run(str, len) % counter_per_array];
        noise=(num_element[0]-temp)/(counter_per_array-1);
        t[0]=temp-noise;
        for (int i = 1; i < hash_num; ++i) {
            temp = data[i][hash[i].Run(str, len) % counter_per_array];
            noise=(num_element[i]-temp)/(counter_per_array-1);
            t[i]=temp-noise;
        }
        std::sort(t, t+hash_num);
        int res = (t[hash_num>>1] + t[(hash_num-1)>>1])>>1;
        int upbound = (1<<bit_per_counter) -1;
        res = res>upbound ? upbound: res;
            res = res<0?0:res;
        return res;
    }
    void clear()
    {
        for (int i = 0; i < hash_num; ++i) {
            for (int j = 0; j < counter_per_array; ++j) {
                data[i][j] = 0;
            }
        }
        delete [] num_element;
        num_element = new int[hash_num];
        memset(num_element, 0, sizeof(int)*hash_num);
    }
};

#endif
