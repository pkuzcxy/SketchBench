#ifndef BASESKETCH_H
#define BASESKETCH_H

template <class Result, class Config>
class BaseSketch {
public:
    virtual void init(const Config& config) = 0;  // should called only ONCE!!
    virtual void insert(const char* str, const unsigned int len) = 0;
    virtual Result query(const char* str, const unsigned int len) = 0;
};

#endif