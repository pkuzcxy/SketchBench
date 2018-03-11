#ifndef DUMMYTASK_H
#define DUMMYTASK_H

#include "BaseTask.h"
#include <cstdio>

template <class Sketch, class Config, class Result, class Dataset>
class DummyTask: public BaseTask<Sketch, Config, Dataset> {
private:
    Dataset dataset;
    Sketch sketch;
public:
    void init(Config& config, const char* datapath) {
        sketch.init(config);
        dataset.init(datapath);
    }
    void run() {
        sketch.insert("123", 3);
        printf("%d\n", sketch.query("123", 3));
        printf("hello world\n");
    }
};

#endif