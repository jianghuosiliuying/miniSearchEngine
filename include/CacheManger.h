#pragma once

#include "Cache.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

namespace mm
{
class MutexLock;
class Configuration;
class CacheManger
{
public:
    static CacheManger * createCacheManger();
    static void destory();
    CacheManger * init();//初始化cache数量，并首次同步cache
    void readFromFile();//从文件中读取cache数据
    void writeToFile();//将cache数据写到文件
    Cache & getCache(size_t);//子线程通过线程编号获取cache
    void periodicUpdate();//周期更新cache
private:
    CacheManger();
    ~CacheManger() {}
private:
    static CacheManger * pCacheManger_;
    Configuration * pconf_;
    string cacheFile_;//cache文件存放路径
    int cacheNum_;//cache数目
    int cacheCap_;//cache容量
    Cache tmplist_;//汇总cache，也是主查询cache
    vector<Cache> cacheList_;//给每个线程一个cache用来存线程产生的热数据
};

}//end of namespace mm
