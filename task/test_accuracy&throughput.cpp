#include "../sketch/CmSketch.h"
#include "../sketch/CSketch.h"
#include "../sketch/CsmSketch.h"
#include "../sketch/CuSketch.h"
#include "../sketch/SketchBase.h"
#include "../sketch/ASketch.h"
#include "../sketch/Count-Mean-MinSketch.h"
#include "../sketch/Bloomfilter.h"
#include "../sketch/SpaceSaving.h"
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
#include <set>


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
bool comp(const item & a,const item & b)
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

void topk_test_spacesaving(vector<string> & v,const int bytesPerStr,int k)
{
    vector<item>  myheap;
    string sketch_name = "spaceSaving";
    SpaceSaving ss(2*k);
    /*topk test insertion*/
    clock_t start,finish;
    start = clock();
    for(auto iter = v.begin();iter != v.end();iter++)
    {
        ss.Insert((*iter).c_str(),bytesPerStr);
    }
    
    finish = clock();
    ofstream throughput_file;
    string throughput_file_name = "top"+to_string(k)+"_throughput_"+sketch_name+\
    +"_"+dataset;
    
    throughput_file.open("./result/"+throughput_file_name+".txt");
    throughput_file << "TotalNum\tTime" << endl;
    throughput_file << v.size() << "\t";
    throughput_file << double(finish-start)/CLOCKS_PER_SEC << "\t";
    throughput_file.close();
    
    /*accuracy test*/
    ofstream topk_file;
    string topk_file_name =  "top"+to_string(k)+"_result_"+sketch_name+\
    +"_"+dataset;
    
    topk_file.open("./result/"+topk_file_name+".txt");
    topk_file<<"top"+to_string(k)<<endl;
    
    
    vector<std::pair<std::string, int> > res = ss.TopK(k);
    for(int i = 0;i<k;++i)
        topk_file << item2idx[res[i].first] <<"\t"<< res[i].second<< endl;
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



void generateTrueHeavyChange(vector<set<string> > & hc,vector<string> &v,int windowSize,double threshold)
{
    auto i = v.begin();
    int j = 0;
    int t  = threshold*windowSize; // threshold for heavy change
    unordered_map<string, int> preWindow;
    unordered_map<string, int> curWindow;
    set<string> key;
    bool firstWindow = true;

    while (i != v.end())
    {
        curWindow[*i]++;
        key.insert(*i);
        j++;
        if(j==windowSize)
        {
            if(firstWindow)
            {
                preWindow.swap(curWindow);
                firstWindow = false;
            }
            else
            {
                set<string> temp_heavyChange;
                for(const auto&p:key)
                {
                    int oldValue = preWindow.find(p)!=preWindow.end() ? preWindow[p]:0;
                    int newValue = curWindow[p];
                    int change = oldValue-newValue<0 ? newValue-oldValue : oldValue-newValue;
                    if (change>t) {
                        temp_heavyChange.insert(p);
                    }
                }
                hc.push_back(temp_heavyChange);
                preWindow.clear();
                preWindow.swap(curWindow);
            }
            j = 0;
        }
        i++;
    }
    
}

void generateEstimateHeavyChange(vector<set<string> > & hc,vector<string> &v,SketchBase<BOBHash,int> &preSketch,SketchBase<BOBHash,int> &curSketch,const int bytesPerStr,int windowSize,double threshold)
{
    auto i = v.begin();
    int j = 0;
    int t  = threshold*windowSize; // threshold for heavy change
    set<string> key;
    bool firstWindow = true;
    bool cur = false;
    while (i != v.end())
    {
        if(cur)
            curSketch.Insert(i->c_str(), bytesPerStr);
        else
            preSketch.Insert(i->c_str(), bytesPerStr);
        key.insert(*i);
        j++;
        if(j==windowSize)
        {
            if(firstWindow)
            {
                firstWindow = false;
                cur = !cur;
            }
            else
            {
                set<string> temp_heavyChange;
                for(const auto&p:key)
                {
                    int preValue = preSketch.Query(p.c_str(),bytesPerStr);
                    int curValue = curSketch.Query(p.c_str(),bytesPerStr);
                    int change = curValue-preValue;
                    change = change>0 ? change:-change;
                    if (change>t) {
                        temp_heavyChange.insert(p);
                    }
                }
                hc.push_back(temp_heavyChange);
                if (cur) preSketch.clear();
                else curSketch.clear();
                cur = !cur;
            }
            j = 0;
        }
        i++;
    }
}

void heavyChangeResult(vector<set<string> > & trueRes,vector<set<string> > & estiRes,SketchBase<BOBHash,int> &sketch)
{
    string sketch_name = sketch.sketch_name;
    ofstream heavyChange_file;
    string heavyChange_file_name =  "heavyChange_"+sketch_name+\
    +"_"+dataset+\
    "_hashnum"+to_string(sketch.hash_num)+\
    "_bitPerCounter"+to_string(sketch.bit_per_counter)+\
    "_counterPerArray"+to_string(sketch.counter_per_array);
    
    heavyChange_file.open("./result/"+heavyChange_file_name+".txt");
    
    heavyChange_file<<"trueHeavyChange\ttrueReport\trecall\tfalseReport"<<endl;

    int vLenth = trueRes.size();
    cout<<vLenth<<endl;
    for(int i =0;i<vLenth;++i)
    {
        int setLenth = estiRes[i].size();
        int recall =  0;
        for(const auto&p:estiRes[i])
        {
            if(trueRes[i].find(p) != trueRes[i].end())
                recall++;
        }
        heavyChange_file<<trueRes[i].size()<<"\t"<<recall<<"\t"<<double(recall)/trueRes[i].size() \
        <<"\t"<<estiRes[i].size()-recall<<endl;
    }
}

void heavyChangeTest(SketchBase<BOBHash,int> &sketch1,SketchBase<BOBHash,int> &sketch2,vector<string> & v,int bytesPerStr)
{
    vector<set<string> > trueRes;
    vector<set<string> > estiRes;
    generateTrueHeavyChange(trueRes,v,1000000,0.0001);
    generateEstimateHeavyChange(estiRes,v,sketch1,sketch2,bytesPerStr,1000000,0.0001);
    heavyChangeResult(trueRes,estiRes,sketch1);
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
    int k =1000;
    for(int i=0;i<k;i++)
        topk_exactRank_file<<item2idx[itemRank[i].s]<<"\t"<<itemRank[i].freq<<endl;
    topk_exactRank_file.close();
    
    

    int p_hashnum,p_bitprecounter,p_counternum;
    //freq_test
    for(int i =4;i<=8;++i)
    {
        p_hashnum = i;
        p_bitprecounter = 16;
        p_counternum = 65536*4/i;
        CmSketch<BOBHash, int> cm(p_hashnum, p_bitprecounter, p_counternum);
        CsmSketch<BOBHash, int> csm(p_hashnum, p_bitprecounter, p_counternum);
        CSketch<BOBHash, int> cs(p_hashnum, p_bitprecounter, p_counternum);
        CuSketch<BOBHash, int> cu(p_hashnum, p_bitprecounter, p_counternum);
        /*ASketch_parameter: elephant item size,max num of items in filter */
        ASketch<BOBHash,int> as(p_hashnum, p_bitprecounter, p_counternum,bytesPerStr,128);
        CountMeanMinSketch<BOBHash, int> cmm(p_hashnum, p_bitprecounter, p_counternum);
        test(v,item2freq,cm,bytesPerStr);
        test(v,item2freq,csm,bytesPerStr);
        test(v,item2freq,cs,bytesPerStr);
        test(v,item2freq,cu,bytesPerStr);
        test(v,item2freq,as,bytesPerStr);
        test(v,item2freq,cmm,bytesPerStr);
    }
    
    
    for(int i = 1;i<=5;++i)
    {
        p_hashnum = 4;
        p_bitprecounter = 16;
        p_counternum = 32768*i;
        CmSketch<BOBHash, int> cm(p_hashnum, p_bitprecounter, p_counternum);
        CsmSketch<BOBHash, int> csm(p_hashnum, p_bitprecounter, p_counternum);
        CSketch<BOBHash, int> cs(p_hashnum, p_bitprecounter, p_counternum);
        CuSketch<BOBHash, int> cu(p_hashnum, p_bitprecounter, p_counternum);
        /*ASketch_parameter: elephant item size,max num of items in filter */
        ASketch<BOBHash,int> as(p_hashnum, p_bitprecounter, p_counternum,bytesPerStr,128);
        CountMeanMinSketch<BOBHash, int> cmm(p_hashnum, p_bitprecounter, p_counternum);
        test(v,item2freq,cm,bytesPerStr);
        test(v,item2freq,csm,bytesPerStr);
        test(v,item2freq,cs,bytesPerStr);
        test(v,item2freq,cu,bytesPerStr);
        test(v,item2freq,as,bytesPerStr);
        test(v,item2freq,cmm,bytesPerStr);
    }

    //topk_test
    topk_test_spacesaving(v,bytesPerStr,k);
    for(int i =4;i<=8;++i)
    {
        p_hashnum = i;
        p_bitprecounter = 16;
        p_counternum = 65536*4/i;
        CmSketch<BOBHash, int> cm(p_hashnum, p_bitprecounter, p_counternum);
        CsmSketch<BOBHash, int> csm(p_hashnum, p_bitprecounter, p_counternum);
        CSketch<BOBHash, int> cs(p_hashnum, p_bitprecounter, p_counternum);
        CuSketch<BOBHash, int> cu(p_hashnum, p_bitprecounter, p_counternum);
        /*ASketch_parameter: elephant item size,max num of items in filter */
        ASketch<BOBHash,int> as(p_hashnum, p_bitprecounter, p_counternum,bytesPerStr,128);
        CountMeanMinSketch<BOBHash, int> cmm(p_hashnum, p_bitprecounter, p_counternum);
        topk_test(v,cm,bytesPerStr,k);
        topk_test(v,csm,bytesPerStr,k);
        topk_test(v,cs,bytesPerStr,k);
        topk_test(v,cu,bytesPerStr,k);
        topk_test(v,as,bytesPerStr,k);
        topk_test(v,cmm,bytesPerStr,k);
    }
    
    for(int i =1;i<=5;++i)
    {
        p_hashnum = 4;
        p_bitprecounter = 16;
        p_counternum = 32768*i;
        CmSketch<BOBHash, int> cm(p_hashnum, p_bitprecounter, p_counternum);
        CsmSketch<BOBHash, int> csm(p_hashnum, p_bitprecounter, p_counternum);
        CSketch<BOBHash, int> cs(p_hashnum, p_bitprecounter, p_counternum);
        CuSketch<BOBHash, int> cu(p_hashnum, p_bitprecounter, p_counternum);
        /*ASketch_parameter: elephant item size,max num of items in filter */
        ASketch<BOBHash,int> as(p_hashnum, p_bitprecounter, p_counternum,bytesPerStr,128);
        CountMeanMinSketch<BOBHash, int> cmm(p_hashnum, p_bitprecounter, p_counternum);
        topk_test(v,cm,bytesPerStr,k);
        topk_test(v,csm,bytesPerStr,k);
        topk_test(v,cs,bytesPerStr,k);
        topk_test(v,cu,bytesPerStr,k);
        topk_test(v,as,bytesPerStr,k);
        topk_test(v,cmm,bytesPerStr,k);
    }
    
    //heavychange_test
    for(int i =1;i<=5;++i)
    {
        p_hashnum = 4;
        p_bitprecounter = 16;
        p_counternum = 32768*i;
        
        
        CmSketch<BOBHash, int> cm1(p_hashnum, p_bitprecounter, p_counternum);
        CsmSketch<BOBHash, int> csm1(p_hashnum, p_bitprecounter, p_counternum);
        CSketch<BOBHash, int> cs1(p_hashnum, p_bitprecounter, p_counternum);
        CuSketch<BOBHash, int> cu1(p_hashnum, p_bitprecounter, p_counternum);
        /*ASketch_parameter: elephant item size,max num of items in filter */
        ASketch<BOBHash,int> as1(p_hashnum, p_bitprecounter, p_counternum,bytesPerStr,128);
        CountMeanMinSketch<BOBHash, int> cmm1(p_hashnum, p_bitprecounter, p_counternum);
        CmSketch<BOBHash, int> cm2(p_hashnum, p_bitprecounter, p_counternum);
        CsmSketch<BOBHash, int> csm2(p_hashnum, p_bitprecounter, p_counternum);
        CSketch<BOBHash, int> cs2(p_hashnum, p_bitprecounter, p_counternum);
        CuSketch<BOBHash, int> cu2(p_hashnum, p_bitprecounter, p_counternum);
        /*ASketch_parameter: elephant item size,max num of items in filter */
        ASketch<BOBHash,int> as2(p_hashnum, p_bitprecounter, p_counternum,bytesPerStr,128);
        CountMeanMinSketch<BOBHash, int> cmm2(p_hashnum, p_bitprecounter, p_counternum);
        heavyChangeTest(cm1,cm2,v,bytesPerStr);
        heavyChangeTest(csm1,csm2,v,bytesPerStr);
        heavyChangeTest(cs1,cs2,v,bytesPerStr);
        heavyChangeTest(cu1,cu2,v,bytesPerStr);
        heavyChangeTest(as1,as2,v,bytesPerStr);
        heavyChangeTest(cmm1,cmm2,v,bytesPerStr);
    }
    for(int i =4;i<=9;++i)
    {
        p_hashnum = i;
        p_bitprecounter = 16;
        p_counternum = 65536*4/i;
        
        
        CmSketch<BOBHash, int> cm1(p_hashnum, p_bitprecounter, p_counternum);
        CsmSketch<BOBHash, int> csm1(p_hashnum, p_bitprecounter, p_counternum);
        CSketch<BOBHash, int> cs1(p_hashnum, p_bitprecounter, p_counternum);
        CuSketch<BOBHash, int> cu1(p_hashnum, p_bitprecounter, p_counternum);
        /*ASketch_parameter: elephant item size,max num of items in filter */
        ASketch<BOBHash,int> as1(p_hashnum, p_bitprecounter, p_counternum,bytesPerStr,128);
        CountMeanMinSketch<BOBHash, int> cmm1(p_hashnum, p_bitprecounter, p_counternum);
        CmSketch<BOBHash, int> cm2(p_hashnum, p_bitprecounter, p_counternum);
        CsmSketch<BOBHash, int> csm2(p_hashnum, p_bitprecounter, p_counternum);
        CSketch<BOBHash, int> cs2(p_hashnum, p_bitprecounter, p_counternum);
        CuSketch<BOBHash, int> cu2(p_hashnum, p_bitprecounter, p_counternum);
        /*ASketch_parameter: elephant item size,max num of items in filter */
        ASketch<BOBHash,int> as2(p_hashnum, p_bitprecounter, p_counternum,bytesPerStr,128);
        CountMeanMinSketch<BOBHash, int> cmm2(p_hashnum, p_bitprecounter, p_counternum);
        heavyChangeTest(cm1,cm2,v,bytesPerStr);
        heavyChangeTest(csm1,csm2,v,bytesPerStr);
        heavyChangeTest(cs1,cs2,v,bytesPerStr);
        heavyChangeTest(cu1,cu2,v,bytesPerStr);
        heavyChangeTest(as1,as2,v,bytesPerStr);
        heavyChangeTest(cmm1,cmm2,v,bytesPerStr);
    }
    return 0;
}
