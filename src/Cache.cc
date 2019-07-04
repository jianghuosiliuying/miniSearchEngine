#include "../include/Cache.h"
#include <iostream>

using namespace std;

namespace mm
{
Cache::Cache(int capacity):capacity_(capacity)
{   cout<<"Cache(int)"<<endl;  }
void Cache::addElement(string word,string json)
{
    auto it=hashmap_.find(word);
    if(it!=hashmap_.end()){//存在该单词
        table_.erase(it->second);//删除原节点
        table_.push_front(make_pair(word,json));//将节点添加到头部
        hashmap_[word]=table_.begin();//存储新节点的迭代器
    }else{//尚不存在该单词
        int sz=table_.size();
        if(sz < capacity_){//未满
            table_.push_front(make_pair(word,json));
            hashmap_[word]=table_.begin();
        }else if(sz==capacity_){//已满
            hashmap_.erase(table_.rbegin()->first);//删除最后一个节点的迭代器
            table_.pop_back();
            table_.push_front(make_pair(word,json));
            hashmap_[word]=table_.begin();
        }
    }
}
string Cache::searchElement(string word)
{
    auto it=hashmap_.find(word);
    if(it!=hashmap_.end()){//存在该word
        table_.push_front(*(it->second));
        table_.erase(it->second);
        hashmap_[word]=table_.begin();
        return table_.begin()->second;
    }else{
        return string();
    }
}
list<pair<string,string>> & Cache::getCacheList()
{  
    return table_;  
}

}//end of namespace mm

