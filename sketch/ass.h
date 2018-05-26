#include <string>
#include <vector>
typedef std::pair<std::string, int> assitem;
template<class Hash, class Unit>
class ass: public SketchBase<Hash, Unit> {
private:
    using SketchBase<Hash, Unit>::hash_num;
    using SketchBase<Hash, Unit>::hash;
    std::vector<std::pair<std::string, int> > counter;
    int m;
    int heapmin;
    int minidx ;
    int sz;
    int byteslen;
public:
    std::vector<assitem> ele[64];
    using SketchBase<Hash, Unit>::sketch_name;
    int  t = 8;
    ass(int hash_num, int bit_per_counter, int counter_per_array,int _m): SketchBase<Hash, Unit>(hash_num, bit_per_counter, counter_per_array)
    {
        strcpy(sketch_name,"ass");
         m = _m;
        heapmin = 1<< 20;
    }
    void Insert(const char* str, const int len)
    {
        byteslen =len;
        const std::string s(str, len);
        if(counter.size()<=m)
        {
            for (int i = (int)counter.size() - 1; i >= 0; --i)
            {
                if (counter[i].first == s)
                {
                    ++counter[i].second;
                    if(heapmin<counter[i].second)
                    {
                        heapmin = counter[i].second;
                        minidx = i;
                    }
                    return;
                }
            }
            counter.push_back(std::make_pair(s, 1));
            heapmin  = 1;
            minidx =counter.size() - 1;
            return;
        }
        else
            Insertass(str,len,1);
    }
    void Insertass(const char* str,int len,int freq)
    {
        
        const std::string s(str, len);
        std::vector<assitem> &zhou = ele[hash[0].Run(str, len) % 64];
        bool find = false;
        for(int i =0;i<zhou.size();++i)
        {
            if(zhou[i].first == s)
            {
                zhou[i].second += freq;
                if(zhou[i].second>heapmin)
                {
                    
                    InsertSpaveSaving(str,zhou[i].second);
                    zhou.erase(zhou.begin()+i);
                }
                find = true;
                return ;
            }
        }
        if(zhou.size()<t)
        {
            zhou.push_back(std::make_pair(s, freq));
        }
        else
        {
            int zhouidx =0;
            int zhoumin = zhou[0].second;
            for(int i = 0;i<t;++i)
            {
                if(zhoumin>zhou[i].second)
                {
                    zhouidx = i;
                    zhoumin =zhou[i].second;
                }
            }
            addSpaceSaving(zhou[zhouidx].first,zhou[zhouidx].second);
            zhou[zhouidx].first = s;
            zhou[zhouidx].second = freq;
        }
    }
    void addSpaceSaving(std::string s,int freq)
    {
        for (int i = (int)counter.size() - 1; i >= 0; --i)
        {
            if(counter[i].first==s)
            {
                counter[i].second += freq;
                return;
            }
        }
    }
    void InsertSpaveSaving(std::string s,int freq)
    {
        assitem tmp =counter[minidx];
        Insertass((tmp.first).c_str(),byteslen,tmp.second);
        counter[minidx].first = s;
        counter[minidx].second = freq;
        heapmin = freq;
        for (int i = (int)counter.size() - 1; i >= 0; --i)
        {
            if(counter[i].second<heapmin)
            {
                heapmin = counter[i].second;
                minidx = i;
            }
        }
    }
    std::vector<std::pair<std::string, int> > TopK(const int k) {
        return std::vector<std::pair<std::string, int> >(counter.begin(), counter.begin() + k);
    }
    Unit Query(const char *str, const int len) {
    }
    void clear()
    {
      
    }
};
