#pragma once

#include "WebPage.h"
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

using namespace std;

namespace mm
{
class Configuration;
class FileProcessor
{
public:
    FileProcessor();
    void processor();//处理文件，提取内容
    vector<string> & getfiles();//返回_vecFilesfile的引用
private:
    void cutRedundantPage();//文章去重
    void InvertIndex();//建立倒排索引
    void traverse();//读取目录，扫描文件存到vector
    void parseXml();//解析xml
    void store();//存网页库和偏移库到文件
    void storeNew();//存去重后的网页库和偏移库到文件，和倒排索引
private:
    vector<string> _vecFilespath;//存每个xml语料库文件的绝对路径
    vector<WebPage> _vecWebpage;//存储每一篇文章
    vector<WebPage> _newvecWebpage;//去重后每一篇文章
    unordered_map<string,unordered_map<int,double>> _InvertIndexTable;//倒排索引表:单词，ID，权重
    unordered_set<string> _stopWord;//停用词
    Configuration * _pconf;
};

}//end of namespace mm
