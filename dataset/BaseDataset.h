#ifndef BASEDATASET_H
#define BASEDATASET_H

template <class Config>
class BaseDataset {
public:
    virtual bool init(Config& config) = 0;
};

#endif