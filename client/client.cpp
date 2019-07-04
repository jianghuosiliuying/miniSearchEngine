#include "../include/json/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <memory>

using namespace std;

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

struct Train
{
    int dataLen;
    char buf[4096];
};

void do_service(int sockfd);

int main()
{
    int peerfd = socket(PF_INET, SOCK_STREAM, 0);
    if(peerfd == -1)
        ERR_EXIT("socket");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.80.128"); //localhost
    //addr.sin_addr.s_addr = INADDR_ANY; //localhost
    addr.sin_port = htons(2000);
    socklen_t len = sizeof addr;
    if(connect(peerfd, (struct sockaddr*)&addr, len) == -1)
        ERR_EXIT("Connect");
#if 0
	char buf[1024];
	memset(buf, 0, sizeof(buf));
        int dataLen;
        int nread;
        recv(peerfd,&dataLen,4,0);
        nread = read(peerfd, buf, dataLen);
	//read(peerfd, buf, sizeof(buf));
	printf("%s\n", buf);
#endif
    do_service(peerfd);
    return 0;
}

void parseJson(const string & data)
{
    Json::Value root;
    Json::Value arr;
    Json::Value elem;
    Json::Reader reader;
    //cout<<data;
    reader.parse(data,root);
    cout<<"similar page --->"<<endl;
    arr=root["files"];
    for(unsigned int i=0;i<arr.size();++i)
    {
        cout<<"title--->"<<arr[i]["title"].asString()<<endl;
        cout<<"url--->"<<arr[i]["url"].asString()<<endl;
        cout<<"summary--->"<<arr[i]["summary"].asString()<<endl;
    }
    cout<<endl;
}

void do_service(int sockfd)
{
    char recvbuf[655350] = {0};
    char sendbuf[655350] = {0};
    while(1)
    {
        int dataLen;
        int nread;
        //struct Train train;
        fgets(sendbuf, sizeof sendbuf, stdin);
        write(sockfd, sendbuf, strlen(sendbuf));//发送单词
#if 0
        train.dataLen=word.size();
        strcpy(train.buf,word.c_str());
        write(sockfd, &train, 4+train.dataLen);//发送单词
		//sleep(5);
#endif
        //read
        recv(sockfd,&dataLen,4,0);
        //cout<<"dataLen="<<dataLen<<endl;
        nread = read(sockfd, recvbuf, dataLen);
        //cout<<"recvbuf="<<recvbuf<<" nread="<<nread<<endl;
        if(nread == -1)
        {
            if(errno == EINTR)
                continue;
            ERR_EXIT("read");
        }
        else if(nread == 0) //代表链接断开
        {
            printf("server close!\n");
            close(sockfd);
            exit(EXIT_SUCCESS);
        }
        
        string data(recvbuf);
        parseJson(data);//解析json
        //printf("receive msg : %s\n", recvbuf);

        memset(recvbuf, 0, sizeof recvbuf);
        memset(sendbuf, 0, sizeof sendbuf);
    }
}

