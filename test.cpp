#include "sketch/CountMinSketch.h"
#include "task/DummyTask.h"
#include "dataset/DummyDataset.h"
#include "config/BaseConfig.h"
#include "hash/BOBHash.h"

int main() {
    DummyTask<CountMinSketch<BOBHash, BaseConfig, int>, BaseConfig, int, DummyDataset> task;
    BaseConfig config;
    config.bit_per_bucket = 4;
    config.bucket_per_array = 1000;
    config.hash_num = 4;
    task.init(config, "456");
    task.run();
    return 0;
}