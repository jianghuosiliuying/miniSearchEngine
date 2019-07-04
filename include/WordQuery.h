#pragma once

#include "WebPage.h"
#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <iostream>

using namespace std;

namespace mm
{
class Configuration;
class WordQuery
{
public:
    static WordQuery * createWordQuery();
    static void destory();
    void init();//初始化网页库，偏移库和索引表
    vector<WebPage> & getpageLib();//获取网页库
    map<int,pair<int,int>> & getOffset();//获取偏移信息
    unordered_map<string,unordered_map<int,double>> & getIndextable();//索引表
private:
    WordQuery(){   cout<<"WordQuery()"<<endl;  }
    ~WordQuery(){}

private:
    static WordQuery * pWordQuery_;
    Configuration * _pconf;
    vector<WebPage> _pageLib;//page
    map<int,pair<int,int>> _offset;//文章号，offset，len
    unordered_map<string,unordered_map<int,double>> _indextable;//索引表:单词，文章号，权重
    unordered_set<string> _stopWord;//停用词
};

}//end of namespace mm
