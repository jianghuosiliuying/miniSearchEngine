#pragma once

#include <vector>
#include <map>

using namespace std;

namespace mm
{
class Configuration;
class DirScanner;
class FileProcessor;
class PageLib
{
public:
    PageLib(Configuration & conf,DirScanner & dirScanner,FileProcessor & fileProcessor);
    void create();//创建网页库
    void store();//存储网页库和位置偏移库
private:
    Configuration & _conf;
    DirScanner & _dirScanner;
    vector<string> _vecFilefiles;//存放格式化后的网页
    map<int,pair<int,int>> _offsetLib;//存放每篇文档在网页库的位置
};

}//end of namespace mm
