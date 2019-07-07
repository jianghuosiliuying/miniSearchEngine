#include "../include/cppjieba/Jieba.hpp"
#include "../include/json/json.h"
#include "../include/TcpConnection.h"
#include "../include/MyTask.h"
#include "../include/WordQuery.h"
#include "../include/Configuration.h"
#include "../include/Thread.h"
#include "../include/Redis.h"
#include <iostream>
#include <map>
#include <vector>

using namespace std;

namespace mm
{
MyTask::MyTask(const string & msg,const mm::TcpConnectionPtr & conn)
: _msg(msg)//待查询单词
, _conn(conn)
,_pwordquery(WordQuery::createWordQuery())//网页库
,_pconf(Configuration::createConfig())
{}

//运行在线程池的某一个子线程中
void MyTask::process()
{
	string response;
    cout<<"i am "<<threadNum<<" thread, id="<<pthread_self()<<endl;
	//decode
#if 1
    Redis * r=new Redis();
    if(!r->connect("192.168.80.128",6379)){
        cout<<"connect error."<<endl;
    }
    string retjson=r->get(_msg);
    if(!retjson.empty()){//主缓存命中
        cout<<threadNum<<" main cache is hit."<<endl;
        response=retjson;
        //cout<<response<<endl;
    }else{//缓存未命中
#endif
	    //compute
        cout<<threadNum<<" main cache is not hit, will compute."<<endl;
        int ret=parseMsg();//查找索引找到每个单词所在文章号，存到_docSet
        if(ret){
            findResultset();//求取单词结果集的交集
            if(!_docResultSet.empty()){
                MyResult result;
                int i=0;
                for(auto & vecW:_docWight)//遍历vector并计算编辑距离，存到优先级队列里
                {
                    int id=_docResultSet[i];
                    double w=computeCOSdistance(vecW);
                    ++i;
                    result._iDocid=id;
                    result._iCos=w;
                    _resultQue.push(result);
                    //cout<<"id="<<id<<" w="<<w<<endl;
                }
                createJson(response);//要返回给客户端的消息
                r->set(_msg,response);//添加热数据
                delete r;
            }else{
                cout<<"don't find the page 1."<<endl;
                //未查找到该网页
                createNoJson(response);//要返回给客户端的消息
            }
        }else{
            cout<<"don't find the page 2."<<endl;
            //未查找到该网页
            createNoJson(response);//要返回给客户端的消息
        }
    }
	//encode
	//_conn->send(response);//由线程池的线程(计算线程)完成数据的发送,在设计上来说，是不合理的
	  				  //数据发送的工作要交还给IO线程(Reactor所在的线程)完成
	  				  //将send的函数的执行延迟到IO线程取操作
    //cout<<response<<endl;
    //以下4句是为php服务
    int sz=response.size();
    string message(to_string(sz));
    message.append("\n").append(response);
    cout<<message<<endl;
	//_conn->sendInLoop(response);
	_conn->sendInLoop(message);
}

void MyTask::createJson(string & response)//建立json
{
    vector<WebPage> & page=_pwordquery->getpageLib();
    Json::Value root;
    Json::Value arr;
    Json::Value elem;
    string title,url,summary;
    for(int i=0;!_resultQue.empty() && i<10;++i)
    {//建立json数组
        title=page[_resultQue.top()._iDocid-1].getTitle();
        url=page[_resultQue.top()._iDocid-1].getUrl();
        summary=page[_resultQue.top()._iDocid-1].getSummary(_word);
        _resultQue.pop();
        elem["title"]=title;
        elem["url"]=url;
        elem["summary"]=summary;
        arr.append(elem);
    }
    root["files"]=arr;
    //Json::FastWriter writer;
    Json::StyledWriter writer;
    response=writer.write(root);//fastwriter自带换行
}

void MyTask::createNoJson(string & response)
{
    Json::Value root;
    Json::Value arr;
    Json::Value elem;
    elem["title"]="404, not found.";
    elem["summary"]="亲, I cannot find what you want. What a pity!";
    elem["url"]="";
    arr.append(elem);
    root["files"]=arr;
    //Json::FastWriter writer;
    Json::StyledWriter writer;
    response=writer.write(root);
}

int MyTask::parseMsg()
{//解析_msg
    Jieba jieba("../include/cppjieba/dict/jieba.dict.utf8",                           
                "../include/cppjieba/dict/hmm_model.utf8",
                "../include/cppjieba/dict/user.dict.utf8",
                "../include/cppjieba/dict/idf.utf8",
                "../include/cppjieba/dict/stop_words.utf8"
               );
    unordered_set<string> stopword=_pconf->getStopWordList();
    WebPage msg(_msg);
    msg.computeFreQuency(jieba,stopword);//建立词频
    map<string,int> & tmp=msg.getWordMap();
    int tf,N=_pwordquery->getpageLib().size()+1;
    double df,idf;
    for(auto & words:tmp)
    {//计算查询语句每个单词的权重
        string word=words.first;
        tf=words.second;
        df=_pwordquery->getIndextable()[word].size()+1.0;
        idf=log(static_cast<double>(N)/(df+1.0))/log(2);
        double wtmp=tf*idf;
        cout<<word<<" tf="<<tf<<" df="<<df<<" idf="<<idf<<" wtmp="<<wtmp<<endl;
        msg.setWordWigh(word,wtmp);
    }
    for(auto & words:tmp)
    {//计算查询语句每个单词的归一权重
        double W=msg.computeWordWight(words.first);
        _word.push_back(words.first);
        _wordWight.push_back(W);
        cout<<words.first<<" 归一权重："<<W<<endl;
    }
    double sumW=0.0;
    for(auto & w:_wordWight)
    {//base模
        sumW+=(w*w);
    }
    _baseMod=sqrt(sumW);
    unordered_map<string,unordered_map<int,double>> & indextable=_pwordquery->getIndextable();
    for(auto & w1:_word)
    {
        auto it=indextable.find(w1);
        if(it==indextable.end()){//不存在该词
            return 0;
        }else{//存在该词
            unordered_map<int,double> indexEm=it->second;
            set<int> t;
            for(auto & index:indexEm)
            {
                t.insert(index.first);
            }
#if 1
            cout<<w1<<" ";
            for(auto & id:t)
            {
                cout<<id<<" ";
            }
            cout<<endl;
#endif
            _docSet.push_back(t);
        }
    }
    return 1;
}

void MyTask::findResultset()
{//求每个词对应文档编号集合的交集
    for(auto & id:_docSet[0])
    {
        _docResultSet.push_back(id);
    }
    for(auto it=_docSet.begin()+1;it!=_docSet.end();++it)
    {
        vector<int> tmp;
        set_intersection((*it).begin(),(*it).end(),_docResultSet.begin(),_docResultSet.end(),inserter(tmp,tmp.begin()));
        _docResultSet.swap(tmp);
    }
    cout<<"输出包含所有单词的文章集合："<<endl;
    for(auto & id:_docResultSet)
    {
        cout<<id<<" ";
    }
    cout<<endl;
    if(!_docResultSet.empty()){//如果交集不为空
        unordered_map<string,unordered_map<int,double>> & indextable=_pwordquery->getIndextable();
        for(auto & id:_docResultSet)
        {//找出每个单词在索引中的权重
            vector<double> tmp;
            for(auto & word:_word)
            {
                double w=indextable[word][id];
                tmp.push_back(w);
            }
            _docWight.push_back(tmp);
        }
    }
}

double MyTask::computeCOSdistance(vector<double> & wight2)
{
    double inner=0.0;
    for(size_t i=0;i!=_wordWight.size();++i)
    {//求内积
        inner+=(_wordWight[i]*wight2[i]);
    }
    double mod2=0.0,sumW=0.0;
    for(auto & w:wight2)
    {
        sumW+=(w*w);
    }
    mod2=sqrt(sumW);
    double cosW=inner/(_baseMod*mod2);
    return cosW;
}

}//end of namespace mm
