#include <iostream>
#include <algorithm>
#include <array>
#include <vector>

using namespace std;

int main()
{
    array<int,5> a{1,1,1,2,2};
    auto it=unique(a.begin(),a.end());
    for(auto iter=a.begin();iter!=it;++iter)
    {
        cout<<*iter<<endl;
    }
    vector<uint64_t> b{1,2,3,4,5,6};
    //cout<<b<<endl;
    return 0;
}

