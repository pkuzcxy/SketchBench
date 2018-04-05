#ifndef DUMMYDATASET_H
#define DUMMYDATASET_H

#include "BaseDataset.h"

template<class Config>
class DummyDataset: public BaseDataset<Config> {
public:
    bool init(Config& config);
};

// implementation

bool DummyDataset::init(Config& config) {}

#endif