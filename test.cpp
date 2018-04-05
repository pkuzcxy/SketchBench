#include "sketch/CountMinSketch.h"
#include "task/DummyTask.h"
#include "dataset/StreamDataset.h"
#include "config/BaseConfig.h"
#include "hash/BOBHash.h"
#include "config/DatasetConfig.h"

int main() {
    DummyTask<CountMinSketch<BOBHash, BaseConfig, int>, BaseConfig, int, StreamDataset<DatasetConfig>, DatasetConfig> task;
    
    BaseConfig config1;
    config1.bit_per_bucket = 4;
    config1.bucket_per_array = 1000;
    config1.hash_num = 4;
    
	DatasetConfig config2;
	config2.filepath = "456";
	config2.bytes_per_str = 6;

    task.init(config1, config2);
    task.run();

    return 0;
}