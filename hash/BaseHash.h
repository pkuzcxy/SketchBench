#ifndef BASEHASH_H
#define BASEHASH_H

template<class Result, class Seed>
class BaseHash {
public:
    virtual Result run(const char* str, unsigned int len) = 0;
    virtual inline void setSeed(const Seed _seed) = 0;
};

#endif