#include "../include/FileProcessor.h"
#include "../include/Configuration.h"
#include "../include/tinyxml2.h"
#include <sys/types.h>
#include <regex>
#include <dirent.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

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
            _vecWebpage.emplace_back(title,url,description,content);
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
    int i=1;//文档编号从1开始
    for(auto & page: _vecWebpage)
    {
        string tmp=page.getDoc(i);
        ofs1<<tmp<<endl;
        //cout<<tmp<<endl;
        int offset=ofs1.tellp();
        int length=tmp.size();
        ofs2<<i<<" "<<offset<<" "<<length<<endl;
        ++i;
    }
    ofs1.close();
    ofs2.close();
    cout<<"store ripage and offset finish."<<endl;
}

vector<string> & FileProcessor::getfiles()
{
    return _vecFilespath;
}

}//end of namespace mm

