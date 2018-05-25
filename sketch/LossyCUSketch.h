#ifndef LOSSYCOUNTINGSKETCH_H_INCLUDED
#define LOSSYCOUNTINGSKETCH_H_INCLUDED

#include "SketchBase.h"

// this sketch would not check counter overflows
// if Unit is not 'unsigned', Query result can be negative

template<class Hash, class Unit>
class LossyCUSketch : public SketchBase<Hash, Unit> {
private:
	using SketchBase<Hash, Unit>::hash_num;
	using SketchBase<Hash, Unit>::counter_per_array;
	using SketchBase<Hash, Unit>::hash;
	using SketchBase<Hash, Unit>::bit_per_counter;
	using SketchBase<Hash, Unit>::data;
	int bucket_size;//每一个bucket的大小
	int bucket_id;//bucket的编号
	int total;//总共加入元素的个数
	double threshold;//查找下界
	double error;//允许的误差范围
	int bucket_num_cnt;//用于计算bucket编号
public:
	char **item;
	using SketchBase<Hash, Unit>::sketch_name;
	LossyCUSketch(int size, double thre, double err, int hash_num, int bit_per_counter, int counter_per_array) : SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array) {
		strcpy(sketch_name, "LossyCUketch");
		bucket_size = size;
		threshold = thre;
		error = err;
		bucket_id = 1;
		total = 0;
		bucket_num_cnt = 0;
		item = (char **)malloc(size * sizeof(char *));
		for (int i = 0;i < size; ++i) {
			item[i] = new char[32];
			item[i][0] = '\0';
		}
	}
	void Insert(const char *str, const int len) {
		Unit min_value = data[0][hash[0].Run(str, len) % counter_per_array];
		for (int i = 1; i < hash_num; ++i) {
			if (data[i][hash[i].Run(str, len) % counter_per_array]<min_value)
				min_value = data[i][hash[i].Run(str, len) % counter_per_array];
		}
		for (int i = 0;i<hash_num;++i) {
			if (data[i][hash[i].Run(str, len) % counter_per_array] == min_value)
				++data[i][hash[i].Run(str, len) % counter_per_array];
		}
		memcpy(item[bucket_num_cnt], str, len);
		item[bucket_num_cnt][len] = '\0';
		bucket_num_cnt++;
		total++;
		if (bucket_num_cnt == bucket_size) {
			for (int j = 0;j < bucket_size; ++j) {
				for (int i = 0;i < hash_num; ++i)
					if (data[i][hash[i].Run(item[j], strlen(item[j])) % counter_per_array] <= bucket_id)
						--data[i][hash[i].Run(item[j], strlen(item[j])) % counter_per_array];
				item[j][0] = '\0';
			}
			bucket_id++;
			bucket_num_cnt = 0;
		}
	}
	Unit Query(const char *str, const int len) {
		Unit res = data[0][hash[0].Run(str, len) % counter_per_array];
		for (int i = 1; i < hash_num; ++i) {
			Unit t = data[i][hash[i].Run(str, len) % counter_per_array];
			res = res < t ? res : t;
		}
		unsigned int upbound = (1 << bit_per_counter) - 1;
		res = res>upbound ? upbound : res;
		return res;
	}
	~LossyCUSketch(){
		for (int i = 0;i < bucket_size; ++i)
			delete []item[i];
	}
    void clear()
    {
        for (int i = 0;i < bucket_size; ++i)
            delete []item[i];
        delete [] item;
        bucket_id = 1;
        total = 0;
        bucket_num_cnt = 0;
        item = (char **)malloc(size * sizeof(char *));
        for (int i = 0;i < bucket_size; ++i) {
            item[i] = new char[32];
            item[i][0] = '\0';
        }
    }
};

#endif // LOSSYCOUNTINGSKETCH_H_INCLUDED
