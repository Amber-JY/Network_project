/*
discription:web服务器主体，继承自TcpSocket
author:zjy
time:2020.10
*/
#include <Winsock2.h>
#include<thread>
#include"TcpSocket.h"
#include <conio.h>
#include"Http.h"

class Server : public TcpSocket {
    public:
    //之后添加异常处理
        Http http;

        Server();
        Server(unsigned short int port, const char *ip);
        ~Server();

        int Run();
        bool recv();
        bool closeserver();

    private:
        
        SOCKADDR_IN addrClient;//保存客户端地址
        int recv_len;
        bool endflag = false;
        char recv_buf[BUFSIZE] = "";
        char send_buf[BUFSIZE] = "";
};