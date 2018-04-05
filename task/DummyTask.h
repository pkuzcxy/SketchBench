#ifndef DUMMYTASK_H
#define DUMMYTASK_H

#include "BaseTask.h"
#include <cstdio>

template <class Sketch, class Config, class Result, class Dataset, class DataConfig>
class DummyTask: public BaseTask<Sketch, Config, Dataset, DataConfig> {
private:
    Dataset dataset;
    Sketch sketch;
public:
    bool init(Config& config1, DataConfig& config2) {
        return sketch.init(config1) && dataset.init(config2);
    }
    void run() {
        sketch.insert("123", 3);
        printf("%d\n", sketch.query("123", 3));
        printf("hello world\n");
    }
};

#endif