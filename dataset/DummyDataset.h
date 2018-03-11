#ifndef DUMMYDATASET_H
#define DUMMYDATASET_H

#include "BaseDataset.h"

class DummyDataset: public BaseDataset {
public:
	void init(const char* filepath);
};

// implementation

void DummyDataset::init(const char* filepath) {}

#endif