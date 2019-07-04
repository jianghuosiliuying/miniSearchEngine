#pragma once

#include <string>
#include <queue>
#include <set>
#include <unordered_map>
#include <memory>

using namespace std;

namespace mm
{
class WordQuery;
class Configuration;
class CacheManger;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

struct MyResult
{
    int _iDocid;//文章编号
    int _iCos;//与查询词的余弦相似度
};

struct MyCompare
{
    bool operator()(const MyResult & lhs,const MyResult & rhs)
    {
        if(lhs._iCos!=rhs._iCos)
            return lhs._iCos<rhs._iCos;//cos值降序
        else{
            return lhs._iDocid>rhs._iDocid;//文档编号升序
        }
    }
};

class MyTask
{
public:
	MyTask(const string & msg,const TcpConnectionPtr & conn);

	//运行在线程池的某一个子线程中
	void process();
    int parseMsg();//解析客户端的句子
    void findResultset();//求单词结果集的交集
    double computeCOSdistance(vector<double> &);//计算cos向量夹角
    void createJson(string & response);//创建json字符串
    void createNoJson(string & response);//创建未找到页面的json字符串
private:
	string _msg;//等待查询的单词
	TcpConnectionPtr _conn;
    CacheManger * pcacheM_;
    WordQuery * _pwordquery;
    Configuration * _pconf;
    vector<string> _word;//待查询词
    vector<double> _wordWight;//待查询词权重集合
    double _baseMod;//待查询词的模
    vector<set<int>> _docSet;//待查询语句每个词所在的文档号set
    vector<int> _docResultSet;//网页库包含所以查询词的交集集合
    vector<vector<double>> _docWight;//交集每篇网页中待查询词权重集合
    priority_queue<MyResult,vector<MyResult>,MyCompare> _resultQue;//查询结果优先级队列
};

}//end of namespace mm
