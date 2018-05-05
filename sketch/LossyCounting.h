#ifndef LOSSYCOUNTING_H_INCLUDED
#define LOSSYCOUNTING_H_INCLUDED

#include<map>
#include<utility>


template<class Unit>
class LossyCounting {
private:
    int bucket_size;//每一个bucket的大小
    int bucket_id;//bucket的编号
    int total;//总共加入元素的个数
    double threshold;//查找下界
    double error;//允许的误差范围
    int bucket_num_cnt;//用于计算bucket编号
    map<const char*,pair<Unit,Unit> > data;
public:
    LossyCounting(int bucketsize,double thres,double err){
        bucket_size=bucketsize;
        threshold=thres;
        error=err;
        bucket_id=1;
        total=0;
        bucket_num_cnt=0;
    }
    void Insert(const char *str, const int len) {
        if(data.find(str)!=data.end()){
            data[str].first++;
        }
        else{
            pair<Unit,Unit> temp(1,bucket_id-1);
            data[str]=temp;
        }
        bucket_num_cnt++;
        total++;
        //当前bucket满了，进入下一个bucket
        if(bucket_num_cnt>=bucket_size){
            map<const char*,pair<Unit,Unit> >::iterator ii=data.begin();
            while(ii!=data.end())
            {
                if(ii->second.first+ii->second.second< bucket_id)
                    data.erase(ii++);
                else
                    ++ii;
            }
            bucket_id++;
            bucket_num_cnt=0;
        }
    }
    Unit Query(const char *str, const int len) {
        return data[str].first;
    }
};

#endif // LOSSYCOUNTING_H_INCLUDED
