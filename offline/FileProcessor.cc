#include "../include/FileProcessor.h"
#include "../include/Configuration.h"
#include "../include/tinyxml2.h"
#include "../include/simhash/Simhasher.hpp"
#include "../include/cppjieba/Jieba.hpp"
#include <sys/types.h>
#include <cmath>
#include <regex>
#include <dirent.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace simhash;

namespace mm
{
FileProcessor::FileProcessor()
:_pconf(Configuration::createConfig())
{   cout<<"FileProcessor()"<<endl;  }

void FileProcessor::processor()
{
    traverse();//读目录
    parseXml();//解析xml
    store();//存网页库和索引
    cutRedundantPage();//文章去重
    InvertIndex();//建立倒排索引
    storeNew();//存储去重后的网页和偏移库，存倒排索引
}
void FileProcessor::traverse()
{//读目录
    string dir=_pconf->getConfigMap()["rss"];
    DIR * d=opendir(dir.c_str());
    if(d==nullptr){
        perror("opendir");
        return;
    }
    struct dirent *p;
    while((p=readdir(d)))
    {
        char path[512]={0};
        if(strstr(p->d_name,".xml")){
            sprintf(path,"%s/%s",dir.c_str(),p->d_name);
            _vecFilespath.push_back(path);
        }
    }
    closedir(d);
}
using namespace tinyxml2;
void FileProcessor::parseXml()
{
    regex re("<[^>]*>"); 
    int docId=1;
    for(auto & k:_vecFilespath)
    {
        XMLDocument doc;
        if(doc.LoadFile(k.c_str())!=0){
            cout<<"load "<<k<<" failed."<<endl;
            return;
        }
        cout<<"will parse "<<k<<" ."<<endl;
        XMLElement * root=doc.RootElement();//根结点
        XMLElement * channel=root->FirstChildElement("channel");
        if(channel==nullptr){
            cout<<k<<" isn't a channel."<<endl;
            continue;
        }
        XMLElement * item=channel->FirstChildElement("item");
        if(item==nullptr){
            cout<<"this isn't a item."<<endl;
            continue;
        }
        while(item)
        {
            string title,url,description,content;
            XMLElement * ptitle=item->FirstChildElement("title");
            if(ptitle && ptitle->GetText()!=nullptr){
                title=ptitle->GetText();
            }else{
                title=string();
            }
            XMLElement * purl=item->FirstChildElement("link");//如果标签存在，就会返回指针
            if(purl && purl->GetText()!=nullptr){
                url=purl->GetText();
            }else{
                url=string();
            }
            XMLElement * pdescription=item->FirstChildElement("description");
            if(pdescription && pdescription->GetText()!=nullptr){
                description=pdescription->GetText();
                description=regex_replace(description,re,"");
            }else{
                description=string();
            }
            XMLElement * pcontent=item->FirstChildElement("content:encoded");
            if(pcontent && pcontent->GetText()!=nullptr){
                content=pcontent->GetText();
                content=regex_replace(content,re,"");
            }else{
                content=string();
            }
            if(title.empty() && !description.empty()){
                stringstream ss(description);
                getline(ss,title);//将第一句话作为title
            }else if(title.empty() && description.empty() && !content.empty()){
                stringstream ss(content);
                getline(ss,title);//将内容第一句话作为标题
            }else if(title.empty() && description.empty() && content.empty()){
                item=item->NextSiblingElement();
                continue;
            }
            _vecWebpage.emplace_back(docId,title,url,description,content);
            ++docId;
            item=item->NextSiblingElement();
        }
        cout<<k<<" parse finish ."<<endl;
    }
    cout<<"all xml parse finish."<<endl;
}
void FileProcessor::store()
{
    cout<<"will store ripage and offset."<<endl;
    string pagepath=_pconf->getConfigMap()["ripepage"];
    string offsetpath=_pconf->getConfigMap()["offset"];
    ofstream ofs1(pagepath);
    ofstream ofs2(offsetpath);
    for(auto & page: _vecWebpage)
    {
        string tmp=page.getDoc();
        ofs1<<tmp;
        //cout<<tmp<<endl;
        int curset=ofs1.tellp();
        int len=tmp.size();
        int offset=curset-len;
        ofs2<<page.getDocID()<<" "<<offset<<" "<<len<<endl;
    }
    ofs1.close();
    ofs2.close();
    cout<<"store ripage and offset finish."<<endl;
}

bool compare(WebPage & a,WebPage & b)
{
    return a.getDocID()<b.getDocID();
}

void FileProcessor::cutRedundantPage()
{//去重网页
    int oldcnt=_vecWebpage.size();
    cout<<"原文档总和："<<oldcnt<<endl;
    Simhasher simhasher("../include/cppjieba/dict/jieba.dict.utf8",
                        "../include/cppjieba/dict/hmm_model.utf8",
                        "../include/cppjieba/dict/idf.utf8",
                        "../include/cppjieba/dict/stop_words.utf8"
                       );
    for(auto & web:_vecWebpage)
    {//为每篇文档计算simhash值
        web.buildU64(simhasher);
    }
    cout<<"buildU64 finish."<<endl;
    sort(_vecWebpage.begin(),_vecWebpage.end());//使用simhash值排序文章
    auto it=unique(_vecWebpage.begin(),_vecWebpage.end());
    for(auto iter=_vecWebpage.begin();iter!=it;++iter)
    {
        _newvecWebpage.push_back(*iter);
    }
    sort(_newvecWebpage.begin(),_newvecWebpage.end(),compare);//重新按id排序
    cout<<"去重后文档总和："<<_newvecWebpage.size()<<endl;
}

using namespace cppjieba;
void FileProcessor::InvertIndex()
{//建立倒排索引
    Jieba jieba("../include/cppjieba/dict/jieba.dict.utf8",                           
                "../include/cppjieba/dict/hmm_model.utf8",
                "../include/cppjieba/dict/user.dict.utf8",
                "../include/cppjieba/dict/idf.utf8",
                "../include/cppjieba/dict/stop_words.utf8"
               );
    cout<<"will create word FreQuency."<<endl;
    for(auto & web:_newvecWebpage)
    {//建立词频
        cout<<"will computing the "<<web.getDocID()<<" word FreQuency."<<endl;
        web.computeFreQuency(jieba);
    }
    cout<<" create word FreQuency finish."<<endl;
    cout<<"will create word Index and docID."<<endl;
    for(auto & web:_newvecWebpage)
    {//建立无权重索引
        for(auto & words:web.getWordMap())
        {
            _InvertIndexTable[words.first].insert(make_pair(web.getDocID(),0.0));
        }
    }
    cout<<"create word Index and docID finish."<<endl;
    int tf,N=_newvecWebpage.size();
    double df,idf;
    for(auto & web1:_newvecWebpage)
    {//计算单词权重
        cout<<"will computing the "<<web1.getDocID()<<" word wight."<<endl;
        map<string,int> & wordmap=web1.getWordMap();
        for(auto & words:wordmap)
        {//遍历每篇文章的每个单词
            string word=words.first;
            tf=words.second;//词频
            df=_InvertIndexTable[word].size();
            idf=log(static_cast<double>(N)/(df+1.0))/log(2);
            double wTmp=tf*idf;//单词权重
            web1.setWordWigh(word,wTmp);//存每个单词的权重
        }
    }
    cout<<"all word wight compute finish."<<endl;
    double W;
    for(auto & web:_newvecWebpage)
    {//计算归一权重
        int id=web.getDocID();
        map<string,int> & wordmap=web.getWordMap();
        for(auto & words:wordmap)
        {
            W=web.computeWordWight(words.first);
            auto it=_InvertIndexTable[words.first].find(id);
            if(it!=_InvertIndexTable[words.first].end()){
                it->second=W;
            }
        }
        cout<<"the "<<id<<" wight is "<<web.getAvgWight()<<endl;
    }
    cout<<"all page index compute finish."<<endl;
}

void FileProcessor::storeNew()
{
    cout<<"will store index to disk."<<endl;
    string newpagepath=_pconf->getConfigMap()["newripepage"];
    string newoffsetpath=_pconf->getConfigMap()["newoffset"];
    string invertIndexpath=_pconf->getConfigMap()["invertIndex"];
    ofstream ofs1(newpagepath);
    ofstream ofs2(newoffsetpath);
    ofstream ofs3(invertIndexpath);
    for(auto & page: _newvecWebpage)
    {
        string tmp=page.getDoc();
        ofs1<<tmp;
        //cout<<tmp<<endl;
        int curset=ofs1.tellp();
        int len=tmp.size();
        int offset=curset-len;
        ofs2<<page.getDocID()<<" "<<offset<<" "<<len<<endl;
    }
    for(auto & index:_InvertIndexTable)
    {
        ofs3<<index.first<<" ";
        unordered_map<int,double> & tmp=index.second;
        for(auto & k:tmp)
        {
            ofs3<<k.first<<" "<<k.second<<" ";
        }
        ofs3<<endl;
    }
    cout<<"store index to disk finish."<<endl;
}

vector<string> & FileProcessor::getfiles()
{
    return _vecFilespath;
}

}//end of namespace mm

