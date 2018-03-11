#ifndef BASEDATASET_H
#define BASEDATASET_H

class BaseDataset {
public:
    virtual void init(const char* filepath) = 0;
};

#endif