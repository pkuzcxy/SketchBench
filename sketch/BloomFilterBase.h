#ifndef BLOOMFILTERBASE_H_INCLUDED
#define BLOOMFILTERBASE_H_INCLUDED

#include <cstdio>

// bit_per_counter is ONLY for error checking

template<class Hash, class Unit>
class BloomFilterBase {
protected:
	const int hash_num, bit_per_counter, counter_of_array;
	static const int MAX_HASH_NUM = 20;
	Unit *data;
	Hash hash[20];
	bool checkParamLegal() {
		if ((sizeof(Unit) << 3) < bit_per_counter) {
			printf("Unit smaller than %d bits.\n", bit_per_counter);
			return false;
		}
		if (hash_num > MAX_HASH_NUM || hash_num <= 0) {
			printf("Invalid hash num %d\n", hash_num);
			return false;
		}
		return true;
	}
	void allocMem() {
		data = new Unit[counter_of_array];
		for (int j = 0; j < counter_of_array; ++j) {
			data[j] = 0;
		}
	}
	void initHash() {
		for (int i = 0; i < hash_num; ++i) {
			hash[i].SetSeed(i + 1000);
		}
	}
public:
	int MemUsageInBit() {
		return bit_per_counter * counter_of_array;
	}
	BloomFilterBase(int _hash_num, int _bit_per_counter, int _counter_of_array) : hash_num(_hash_num), bit_per_counter(_bit_per_counter), counter_of_array(_counter_of_array) {
		if (checkParamLegal()) {
			allocMem();
			initHash();
		}
	}
	~BloomFilterBase() {
		delete data;
	}
};

#endif // BLOOMFILTERBASE_H_INCLUDED
