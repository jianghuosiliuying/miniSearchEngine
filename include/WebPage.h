#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

namespace mm
{

class WebPage
{
public:
    WebPage(string & title,string & url,string & description,string & content);
    string getDoc(int);
private:
    string _docTitle;
    string _docUrl;
    string _docDescription;
    string _docContent;
    string _docSummary;//文档摘要
    vector<string> _topWord;//词频前20的词
    map<string,int> _wordMap;//每篇文档的所有词和词频，包括停用词
};

}//end of namespace mm
