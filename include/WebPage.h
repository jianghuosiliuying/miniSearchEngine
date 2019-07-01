#pragma once

#include "cppjieba/Jieba.hpp"
#include "simhash/Simhasher.hpp"
#include <string>
#include <map>
#include <vector>

using namespace std;
using namespace cppjieba;
using namespace simhash;

namespace mm
{
class WebPage
{
    friend bool operator==(const WebPage & lhs,const WebPage & rhs);
    friend bool operator<(const WebPage & lhs,const WebPage & rhs);
public:
    WebPage(int id,string & title,string & url,string & description,string & content);
    int getDocID();
    string getDoc();
    string getContent();
    void buildU64(Simhasher &);
    void computeFreQuency(Jieba &);
    int searchWord(string);//查该词出现的次数
    void setWordWigh(string,double);//为单词添加权重值
    void computeAvgWigh();//计算归一权重值
    double getAvgWight();//获取全文归一化权重
    double computeWordWight(string);//获取每个词的权重
    map<string,int> & getWordMap();
private:
    int _docId;
    string _docTitle;
    string _docUrl;
    string _docDescription;
    string _docContent;
    string _docSummary;//文档摘要
    uint64_t _u64;//该文档的simhash值
    double _avgWight;//全文权重 sqrt(wAll)
    unordered_map<string,double> _wordWight;//每个词的权重 w
    map<string,int> _wordMap;//每篇文档的所有词和词频
};

}//end of namespace mm
