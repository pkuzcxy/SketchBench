#ifndef BASETASK_H
#define BASETASK_H

template <class Sketch, class Config, class Dataset, class DataConfig>
class BaseTask {
public:
    virtual bool init(Config& config1, DataConfig& config2) = 0;
    virtual void run() = 0;
};


#endif