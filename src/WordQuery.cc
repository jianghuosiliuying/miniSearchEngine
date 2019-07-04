#include "../include/WordQuery.h"
#include "../include/Configuration.h"
#include "../include/cppjieba/Jieba.hpp"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace cppjieba;

namespace mm
{

WordQuery * WordQuery::createWordQuery()
{
    if(pWordQuery_==nullptr){
        pWordQuery_=new WordQuery();
        atexit(destory);
    }
    return pWordQuery_;
}
void WordQuery::destory()
{
    if(pWordQuery_)
        delete pWordQuery_;
}
void WordQuery::init()
{
    cout<<"will init WordQuery."<<endl;
    _pconf=Configuration::createConfig();
    _stopWord=_pconf->getStopWordList();//获取停用词
    string newpagePath=_pconf->getConfigMap()["newripepage"];
    string newoffsetPath=_pconf->getConfigMap()["newoffset"];
    string invertPath=_pconf->getConfigMap()["invertIndex"];
    int id,offset,len;
    cout<<"will build offset."<<endl;
    ifstream ifs1(newoffsetPath);//构建偏移库
    while(ifs1>>id>>offset>>len)
    {
        _offset[id]=(make_pair(offset,len));
        //cout<<id<<" "<<offset<<" "<<len<<endl;
    }
    ifs1.close();
    cout<<"will build pageLib."<<endl;
    string txt,url,title,description,content;
    ifstream ifs2(newpagePath);
    for(auto & set:_offset)
    {
        offset=set.second.first;
        len=set.second.second;
        char buf[65535]={0};
        ifs2.seekg(offset,ifs2.beg);
        ifs2.read(buf,len);
        txt=buf;
        int beg=txt.find("<docid>")+7;
        int end=txt.find("</docid>",beg);
        id=stoi(txt.substr(beg,end-beg));
        //cout<<"id="<<id<<endl;
        beg=txt.find("<url>")+5;
        end=txt.find("</url>",beg);
        url=txt.substr(beg,end-beg);
        //cout<<"url="<<endl<<url<<endl;
        beg=txt.find("<title>")+7;
        end=txt.find("</title>",beg);
        title=txt.substr(beg,end-beg);
        //cout<<"title="<<endl<<title<<endl;
        beg=txt.find("<description>")+13;
        end=txt.find("</description>",beg);
        description=txt.substr(beg,end-beg);
        //cout<<"description="<<endl<<description<<endl;
        beg=txt.find("<content>")+9;
        end=txt.find("</content>",beg);
        content=txt.substr(beg,end-beg);
        //cout<<"content="<<endl<<content<<endl;
        _pageLib.emplace_back(id,title,url,description,content);
    }
    cout<<"网页库总数为："<<_pageLib.size()<<endl;
    string line,word;
    double w;
    cout<<"will build invertIndex."<<endl;
    ifstream ifs3(invertPath);//构建索引表
    while(getline(ifs3,line))
    {
        stringstream ss(line);
        ss>>word;//单词
        //cout<<word<<" ";
        while(ss>>id>>w)
        {
            _indextable[word].insert(make_pair(id,w));
            //cout<<id<<" "<<w<<" ";
        }
        //cout<<endl;
    }
    ifs3.close();
#if 0
    cout<<"will build word Frequency."<<endl;
    Jieba jieba("../include/cppjieba/dict/jieba.dict.utf8",                           
                "../include/cppjieba/dict/hmm_model.utf8",
                "../include/cppjieba/dict/user.dict.utf8",
                "../include/cppjieba/dict/idf.utf8",
                "../include/cppjieba/dict/stop_words.utf8"
               );
    for(auto & web:_pageLib)
    {//计算词频
        web.computeFreQuency(jieba,_stopWord);
    }
#endif
    cout<<"the WordQuery init finish."<<endl;
}
vector<WebPage> & WordQuery::getpageLib()
{
    return _pageLib;
}
map<int,pair<int,int>> & WordQuery::getOffset()
{
    return _offset;
}
unordered_map<string,unordered_map<int,double>> & WordQuery::getIndextable()
{
    return _indextable;
}

WordQuery * WordQuery::pWordQuery_=createWordQuery();//饱汉模式

}//end of namespace mm
