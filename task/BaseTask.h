#ifndef BASETASK_H
#define BASETASK_H

template <class Sketch, class Config, class Dataset>
class BaseTask {
public:
	virtual void init(Config& config, const char* datapath) = 0;
	virtual void run() = 0;
};


#endif