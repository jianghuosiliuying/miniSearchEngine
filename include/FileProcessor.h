#pragma once

#include "WebPage.h"
#include <vector>
#include <string>
#include <map>

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
    void traverse();//读取目录，扫描文件存到vector
    void parseXml();//解析xml
    void store();//存网页库和偏移库到文件
private:
    vector<string> _vecFilespath;//存每个xml语料库文件的绝对路径
    vector<WebPage> _vecWebpage;//存储每一篇文章
    Configuration * _pconf;
};

}//end of namespace mm
