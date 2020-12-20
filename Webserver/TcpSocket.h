/*
discribtion:socket类
author:zjy
time:2020.10
*/
#include"Config.h"
#include <Winsock2.h>
#include <list>
#include <iostream>
#include <conio.h>




class TcpSocket{
    public:
        //默认建立tcpsocket
        SOCKET socket_;
        SOCKET socket_srv;
        sockaddr_in addrHost;
        
        //int num_conn = 0;//连接数
        //list<SOCKET> List_conn;//链接队列

        TcpSocket() {};
        TcpSocket(SOCKET);
  
        bool create();
        bool bind();
        bool listen(int backlog = BACKLOG);
        bool accept(sockaddr_in* client_addr);
        bool close();
};
