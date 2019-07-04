#pragma once

#include <iostream>
#include <string>
#include <map>
#include <unordered_set>

using namespace std;

namespace mm
{
class Configuration
{
public:
    static Configuration * createConfig();
    static void destroy();
    Configuration * init(const string & filepath);
    map<string,string> & getConfigMap();
    unordered_set<string> & getStopWordList();
private:
    Configuration(){    cout<<"Configuration()"<<endl;  }
    ~Configuration(){}
private:
    static Configuration * _pconf;
    string _filepath;
    map<string,string> _configMap;
    unordered_set<string> _stopWordList;//停用词词集
};

}//end of namespace mm
