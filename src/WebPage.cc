#include "../include/WebPage.h"
#include <iostream>
#include <fstream>

namespace mm
{
WebPage::WebPage(string & content)
:_docId(0)
,_docTitle(string())
,_docUrl(string())
,_docDescription(string())
,_docContent(content)
{}

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
string WebPage::getTitle()
{
    return _docTitle;
}
string WebPage::getUrl()
{
    return _docUrl;
}
string WebPage::getSummary(vector<string> & word)
{
    createSummary(word);
    return _docSummary;
}
void WebPage::createSummary(vector<string> & word)
{//截取单词所在的一行
    string content=_docDescription+"\n"+_docContent;
    string line;
    stringstream ss(content);
    while(getline(ss,line))
    {
        for(auto & w:word)
        {
            size_t cur=line.find(w);//无符号数
            int n=cur;
            if(cur!=string::npos){
                string w;
                //cout<<"sz="<<line.size()<<" cur="<<cur<<endl;
                if((n-150)<0 && (n+150)<(int)line.size()){
                    w=line.substr(0,(cur+150));//头部短，尾部长
                    _docSummary.append(w).append("...<br/>");
                }else if((n-150)<0 && (n+150)>=(int)(line.size())){
                    w=line.substr(0,line.size());//头部短，尾部短
                    _docSummary.append(w).append("...<br/>");
                }else if((n-150)>=0 && (n+150)>=(int)(line.size())){
                    w=line.substr(n-150,(line.size()-cur+150));//头部长，尾部短
                    _docSummary.append(w).append("...<br/>");
                }else{
                    w=line.substr(cur-150,300);
                    _docSummary.append(w).append("...<br/>");
                }
                //cout<<_docSummary<<endl;
            }
        }
    }
}

void WebPage::setDocId(int id)
{
    _docId=id;
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
void WebPage::computeFreQuency(Jieba & jieba,unordered_set<string> & stopWord)
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
        auto it=stopWord.find(word);
        if(it==stopWord.end())
        {//如果不是停用词，则存入词频库
            ++_wordMap[word];
        }
    }
}

}//end of namespace mm

