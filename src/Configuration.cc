#include "../include/Configuration.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace mm
{
Configuration * Configuration::createConfig()
{
    if(_pconf==nullptr){
        _pconf=new Configuration();
        atexit(destroy);
    }
    return _pconf;
}
void Configuration::destroy()
{
    if(_pconf){
        delete _pconf;
    }
}
Configuration * Configuration::init(const string & filepath)
{
    string s1,s2;
    ifstream ifs(filepath);
    while(ifs>>s1>>s2)
    {
        _configMap.insert(make_pair(s1,s2));
        //cout<<s1<<" "<<s2<<endl;
    }
    ifs.close();
    string word;
    ifs.open(_configMap["stopword"]);
    while(ifs>>word)
    {
        _stopWordList.insert(word);
    }
    ifs.close();
    return _pconf;
}
map<string,string> & Configuration::getConfigMap()
{
    return _configMap;
}
unordered_set<string> & Configuration::getStopWordList()
{
    return _stopWordList;
}

Configuration * Configuration::_pconf=createConfig();//饱汉模式

}//end of namespace mm

