#include "../sketch/CmSketch.h"
#include "../sketch/CSketch.h"
#include "../sketch/CsmSketch.h"
#include "../sketch/CuSketch.h"
#include "../sketch/SketchBase.h"
#include "../sketch/ASketch.h"
#include "../sketch/Count-Mean-MinSketch.h"
#include "../sketch/Bloomfilter.h"
#include "../dataset/StreamData.h"
#include "../hash/BOBHash.h"

#include <unordered_map>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>


using namespace std;

string dataset;
void test(vector<string> & v,unordered_map<string, int> & item2freq, SketchBase<BOBHash,int> &sketch,const int bytesPerStr)
{
    
    string sketch_name = sketch.sketch_name;
    /*throuput test*/
    clock_t start,finish;
    start = clock();
    for(auto iter = v.begin();iter!=v.end();iter++)
        sketch.Insert(iter->c_str(), bytesPerStr);
    finish = clock();
    ofstream throughput_file;
    string throughput_file_name = "throughput_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitpercounter"+to_string(sketch.bit_per_counter)+\
    "_counterperarray"+to_string(sketch.counter_per_array);
    
    throughput_file.open("./result/"+throughput_file_name+".txt");
    throughput_file << "TotalNum\tTime" << endl;
    throughput_file << v.size() << "\t";
    throughput_file << double(finish-start)/CLOCKS_PER_SEC << "\t";
    throughput_file.close();

    /*accuracy test*/
    ofstream frequency_file;
    string frequency_file_name =  "frequency_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitPerCounter"+to_string(sketch.bit_per_counter)+\
    "_counterPerArray"+to_string(sketch.counter_per_array);
    frequency_file.open("./result/"+frequency_file_name+".txt");
    frequency_file<<"exact\testimated"<<endl;
    for (const auto& p: item2freq) {
        frequency_file << p.second << "\t";
        frequency_file << sketch.Query(p.first.c_str(), bytesPerStr) << "\t";
        frequency_file << endl;
    }
    frequency_file.close();
}


class item
{
public :
    string s;
    int freq;
};
bool comp(item & a,item & b)
{
    return a.freq>b.freq;
}
unordered_map<string, int> item2idx;
void topk_test(vector<string> & v, SketchBase<BOBHash,int> &sketch,const int bytesPerStr,int k)
{
    vector<item>  myheap;
    string sketch_name = sketch.sketch_name;
    
    /*topk test insertion*/
    clock_t start,finish;
    start = clock();
    for(auto iter = v.begin();iter != v.end();iter++)
    {
        bool find  = false;
        for (auto HeapIter = myheap.begin();HeapIter!=myheap.end();HeapIter++)
        {
            if ((*HeapIter).s == *iter )
            {
                (*HeapIter).freq += 1;
                find =true;
                break;
            }
        }
        if (!find)
        {
            item newitem;
            newitem.s = *iter;
            newitem.freq = 1;
            if(myheap.size() < k)
                myheap.push_back(newitem);
            else
            {
                int QueryRes = sketch.Query(newitem.s.c_str(), bytesPerStr)+1;
                int minIdx = 0;
                int minFreq = myheap[0].freq;
                for(int i = 0 ;i<k;++i)
                {
                    if (myheap[i].freq<minFreq)
                    {
                        minIdx  = i;
                        minFreq = myheap[i].freq;
                    }
                }
                if(minFreq<QueryRes)
                {
                    int insertTime =minFreq-sketch.Query(myheap[minIdx].s.c_str(), bytesPerStr);
                    for(int j=0;j<insertTime;++j)
                        sketch.Insert(myheap[minIdx].s.c_str(), bytesPerStr);
                    myheap[minIdx].s = *iter;
                    myheap[minIdx].freq = QueryRes;
                }
                else
                    sketch.Insert(newitem.s.c_str(), bytesPerStr);
            }
        }
        
    }
        
    finish = clock();
    ofstream throughput_file;
    string throughput_file_name = "top"+to_string(k)+"_throughput_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitpercounter"+to_string(sketch.bit_per_counter)+\
    "_counterperarray"+to_string(sketch.counter_per_array);
    
    throughput_file.open("./result/"+throughput_file_name+".txt");
    throughput_file << "TotalNum\tTime" << endl;
    throughput_file << v.size() << "\t";
    throughput_file << double(finish-start)/CLOCKS_PER_SEC << "\t";
    throughput_file.close();
    
    /*accuracy test*/
    ofstream topk_file;
    string topk_file_name =  "top"+to_string(k)+"_result_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitPerCounter"+to_string(sketch.bit_per_counter)+\
    "_counterPerArray"+to_string(sketch.counter_per_array);
    
    topk_file.open("./result/"+topk_file_name+".txt");
    topk_file<<"top"+to_string(k)<<endl;
    sort(myheap.begin(), myheap.end(),comp);
    
    
    for (auto HeapIter = myheap.begin();HeapIter!=myheap.end();HeapIter++)
    {
        topk_file << item2idx[HeapIter->s] <<"\t"<< HeapIter->freq<< endl;
    }
    
    topk_file.close();
    
}

void multiset_test(vector<string> & v, SketchBase<BOBHash,int> &sketch,const int bytesPerStr,int k)
{
    
    string sketch_name = sketch.sketch_name;
    set<string> insertedItems;
    
    for(auto iter = v.begin();iter!=v.end();iter++)
    {
        
        if(insertedItems.find(*iter) == insertedItems.end())
        {
            insertedItems.insert(*iter);
            int insertTimes = item2idx[*iter]%k;
            while (insertTimes--)
                sketch.Insert(iter->c_str(), bytesPerStr);
        }
    }
    
    /*multiset test*/
    ofstream multiset_file;
    string multiset_file_name =  "multiset_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitPerCounter"+to_string(sketch.bit_per_counter)+\
    "_counterPerArray"+to_string(sketch.counter_per_array);
    multiset_file.open("./result/"+multiset_file_name+".txt");
    multiset_file<<"exact\ttrueSet\testimatedSet"<<endl;
    
    int i =0;
    int trueCount = 0;
    for (const auto& p: item2idx) {
        multiset_file << p.second << "\t";
        multiset_file << p.second%k << "\t";
        multiset_file << sketch.Query(p.first.c_str(), bytesPerStr) << "\t";
        multiset_file << endl;
        i++;
        if(p.second%k == sketch.Query(p.first.c_str(), bytesPerStr))
            trueCount++;
    }
    cout<<i<<" "<<trueCount<<" "<<double(trueCount)/i<<endl;
    multiset_file.close();
}

void BF_multiset_test(vector<string> & v,SketchBase<BOBHash,int> &sketch,const int bytesPerStr,int k)
{
    
    string sketch_name = sketch.sketch_name;
    set<string> insertedItems;
    vector<BloomFilter<BOBHash,int> > bf_list;
    for(int i =0;i<k;++i)
    {
        BloomFilter<BOBHash,int> bf(1,1,65536*4*16) ;
        bf_list.push_back(bf);
    }
    for(auto iter = v.begin();iter!=v.end();iter++)
    {
        
        if(insertedItems.find(*iter) == insertedItems.end())
        {
            insertedItems.insert(*iter);
            int insertTimes = item2idx[*iter]%k;
            bf_list[insertTimes].Insert(iter->c_str(), bytesPerStr);
        }
    }
    /*multiset test*/
    ofstream multiset_file;
    string multiset_file_name =  "multiset_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitPerCounter"+to_string(sketch.bit_per_counter)+\
    "_counterPerArray"+to_string(sketch.counter_per_array);
    multiset_file.open("./result/"+multiset_file_name+".txt");
    multiset_file<<"exact\ttrueSet\testimatedSet"<<endl;
    
    int i =0;
    int trueCount = 0;
    for (const auto& p: item2idx) {
        multiset_file << p.second << "\t";
        multiset_file << p.second%k << "\t";
        int queryRes = 0;
        for (int i=0; i<k; ++i)
        {
            if(bf_list[i].Query((p.first).c_str(),bytesPerStr))
               {
                   queryRes = i;
                   break;
               }
        }
        multiset_file << queryRes << "\t";
        multiset_file << endl;
        i++;
        if(p.second%k == queryRes)
            trueCount++;
    }
    cout<<i<<" "<<trueCount<<" "<<double(trueCount)/i<<endl;
    multiset_file.close();
}





int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        cout << "Please input filepath" << endl;
        return -1;
    }
    // Data Source
    dataset = argv[1];
    const int bytesPerStr = 4;
    StreamData dat(argv[1], bytesPerStr);
    unordered_map<string, int> item2freq;
    int idx =0;
    char str[bytesPerStr];
    vector<string> v;
    while (dat.GetNext(str))
    {
        v.push_back(string(str, bytesPerStr));
        ++item2freq[string(str, bytesPerStr)];
        item2idx[string(str, bytesPerStr)]= idx++;
    }
    
    
    
    vector<item> itemRank;
    for (auto it = item2freq.begin(); it != item2freq.end(); it++)
    {
        item newitem;
        newitem.freq =it->second;
        newitem.s= it->first;
        itemRank.push_back(newitem);
    }
        
    sort(itemRank.begin(), itemRank.end(),comp);
    ofstream topk_exactRank_file;
    topk_exactRank_file.open("./result/topk_exactRank_"+dataset+".txt");
    topk_exactRank_file<<"id\tfreq"<<endl;
    int k =100;
    for(int i=0;i<k;i++)
        topk_exactRank_file<<item2idx[itemRank[i].s]<<"\t"<<itemRank[i].freq<<endl;
    topk_exactRank_file.close();
    
    

    // sketches
    CmSketch<BOBHash, int> cm(4, 16, 65536);
    CsmSketch<BOBHash, int> csm(4, 16, 65536);
    CSketch<BOBHash, int> cs(4, 16, 65536);
    CuSketch<BOBHash, int> cu(4, 16, 65536);

    /*ASketch_parameter: elephant item size,max num of items in filter */
    ASketch<BOBHash,int> as(4,16,65536,bytesPerStr,128);
    CountMeanMinSketch<BOBHash, int> cmm(4,16,65536);
    
    
    //test
    /*
    test(v,item2freq,cm,bytesPerStr);
    test(v,item2freq,csm,bytesPerStr);
    test(v,item2freq,cs,bytesPerStr);
    test(v,item2freq,cu,bytesPerStr);
    test(v,item2freq,as,bytesPerStr);
    test(v,item2freq,cmm,bytesPerStr);
     */
    
    //topk_test(v,cm,bytesPerStr,k);
    
    //multiset_test(v,cm,bytesPerStr,k);
    BloomFilter<BOBHash,int> bf(1,1,65536*4*16) ;
    BF_multiset_test(v,bf,bytesPerStr,k);
    
    return 0;
}
