#include "../include/Configuration.h"
#include "../include/FileProcessor.h"
#include <iostream>

using namespace std;
using namespace mm;

int main()
{
    Configuration * pconf=Configuration::createConfig()->init("../conf");
    FileProcessor dir;
    vector<string> & file=dir.getfiles();
    for(auto & k:file)
    {
        cout<<k<<endl;
    }
    dir.processor();
    return 0;
}

