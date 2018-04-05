#ifndef STREAMDATASET_H
#define STREAMDATASET_H

#include "BaseDataset.h"
#include "stdio.h"
#include "stdlib.h"

template<class Config>
class StreamDataset: public BaseDataset<Config> {
private:
	FILE* fp;
	int bytes_per_str;
public:
    bool init(Config& config);
    ~StreamDataset();
    bool getNext(char* str);
    void reset();
};

// implementation

template<class Config>
bool StreamDataset<Config>::init(Config& config) {
	if (!(fp = fopen(config.filepath, "rb"))) {
		printf("wrong file path\n");
		return false;
	}
	bytes_per_str = config.bytes_per_str;
	fseek(fp, 0, SEEK_END);
	long long size = ftell(fp);
	fseek(fp, 0, 0);
	return size % bytes_per_str;
}

template<class Config>
StreamDataset<Config>::~StreamDataset() {
	fclose(fp);
}

template<class Config>
bool StreamDataset<Config>::getNext(char* str) {
	int result = fread(str, sizeof(char), bytes_per_str, fp);
	return result == bytes_per_str;
}

template<class Config>
void StreamDataset<Config>::reset() {
	fseek(fp, 0, 0);
}

#endif