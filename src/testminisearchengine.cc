#include "../include/WordQueryServer.h"
#include "../include/Configuration.h"
#include <iostream>

using namespace std;
using namespace mm;

int main()
{
    SpellcorrectServer server("../conf");
    //cout<<"1"<<endl;

    server.start();

	return 0;
}
