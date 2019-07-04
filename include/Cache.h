#pragma once

#include <unordered_map>
#include <list>
#include <string>

using namespace std;

namespace mm
{
class Cache
{
public:
    Cache(int capacity);
    void addElement(string,string);//添加元素
    string searchElement(string);//使用元素,存在返回json，不存在返回空串
    list<pair<string,string>> & getCacheList();
    //Cache & operator=(Cache & rhs);
    //Cache & operator=(Cache && rhs);
private:
    int capacity_;//cache容量
    list<pair<string,string>> table_;//单词和json
    unordered_map<string,list<pair<string,string>>::iterator> hashmap_;//list的单词查找索引,实现O(1)的辅助索引
};

}//end of namespace mm
