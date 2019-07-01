#include "../include/WebPage.h"
#include <iostream>
#include <fstream>

namespace mm
{
WebPage::WebPage(int id,string & title,string & url,string & description,string & content)
:_docId(id)
,_docTitle(title)
,_docUrl(url)
,_docDescription(description)
,_docContent(content)
{
    //cout<<"WebPage()"<<endl; 
}

int WebPage::getDocID()
{
    return _docId;
}

string WebPage::getDoc()
{
    string txt="<doc>\n<docid>"+to_string(_docId)
        +"</docid>\n<url>"+_docUrl+
        +"</url>\n<title>"+_docTitle
        +"</title>\n<description>"+_docDescription
        +"</description>\n<content>"+_docContent
        +"</content>\n</doc>\n";
    return txt;
}
string WebPage::getContent()
{
    return _docDescription+_docContent;
}

int WebPage::searchWord(string word)
{//存在该词返回次数，否则返回0
    auto it=_wordMap.find(word);
    if(it!=_wordMap.end()){
        return it->second;
    }else{
        return 0;
    }
}

void WebPage::computeAvgWigh()
{
    double wAll=0.0;
    for(auto & w:_wordWight)
    {
        wAll+=(w.second*w.second);
    }
    _avgWight=sqrt(wAll);
}
double WebPage::getAvgWight()
{
    return _avgWight;
}
double WebPage::computeWordWight(string word)
{
    computeAvgWigh();
    double w=_wordWight.find(word)->second;
    return w/_avgWight;
}

void WebPage::setWordWigh(string word,double wight)
{
    _wordWight[word]=wight;
}

void WebPage::buildU64(Simhasher & simhasher)
{
    string content=_docDescription+_docContent;
#if 0
    Simhasher simhasher("../include/cppjieba/dict/jieba.dict.utf8",
                        "../include/cppjieba/dict/hmm_model.utf8",
                        "../include/cppjieba/dict/idf.utf8",
                        "../include/cppjieba/dict/stop_words.utf8"
                       );
#endif
    size_t topN=5;
    //vector<pair<string,double>> res;
    //vector<pair<uint64_t,double>> res1;
    //simhasher.extract(content,res,topN);//存的是每一个词和对应的浮点值
    simhasher.make(content,topN,_u64);//存的是全文的simhash值
    cout<<_u64<<endl;
    //simhasher.make(content,topN,res1);//存的是每个词的simhash值和一个该词对应的浮点值
#if 0
    for(auto & s:res)
    {
        cout<<"关键词序列是:"<<s.first<<" "<<s.second<<endl;
        cout<<"simhash值是:"<<_u64<<endl;
    }
    for(auto & s:res1)
    {
        cout<<"关键词simhash是:"<<s.first<<" "<<s.second<<endl;
    }
#endif
}
map<string,int> & WebPage::getWordMap()
{
    return _wordMap;
}

bool operator==(const WebPage & lhs,const WebPage & rhs)
{
    return lhs._u64==rhs._u64;
}

bool operator<(const WebPage & lhs,const WebPage & rhs)
{
    return lhs._u64<rhs._u64;
}

using namespace cppjieba;
void WebPage::computeFreQuency(Jieba & jieba)
{
    string content=_docDescription+_docContent;
#if 0
    Jieba jieba("../include/cppjieba/dict/jieba.dict.utf8",                           
                "../include/cppjieba/dict/hmm_model.utf8",
                "../include/cppjieba/dict/user.dict.utf8",
                "../include/cppjieba/dict/idf.utf8",
                "../include/cppjieba/dict/stop_words.utf8"
               );
#endif
    vector<string> res;
    jieba.Cut(content,res);
    for(auto & word:res)
    {//建立词频
        ++_wordMap[word];
    }
}

}//end of namespace mm

