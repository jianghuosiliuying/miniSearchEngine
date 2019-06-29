#include "../include/WebPage.h"
#include <iostream>

using namespace std;

namespace mm
{
WebPage::WebPage(string & title,string & url,string & description,string & content)
:_docTitle(title)
,_docUrl(url)
,_docDescription(description)
,_docContent(content)
{
    //cout<<"WebPage()"<<endl; 
}

string WebPage::getDoc(int docID)
{
    string txt="<doc>\n<docid>"+to_string(docID)
                +"</docid>\n<url>"+_docUrl+
                +"</url>\n<title>"+_docTitle
                +"</title>\n<description>"+_docDescription
                +"</description>\n<content>"+_docContent
                +"</content>\n</doc>\n";
    return txt;
}


}//end of namespace mm

