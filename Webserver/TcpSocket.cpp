/*
discribtion:socket类实现
author:zjy
time:2020.10
*/
#include"TcpSocket.h"


/*
para   none
return true:success false:error
discription:启动socket，创建tcp链接
*/
bool TcpSocket::create()
{
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    return TRUE;
}

bool TcpSocket::bind()
{
    addrHost.sin_family = AF_INET;
    addrHost.sin_port = htons(PORT);
    addrHost.sin_addr.S_un.S_addr =INADDR_ANY;
    if(::bind(socket_, (const sockaddr*)&addrHost, sizeof(addrHost)) == SOCKET_ERROR)
    {
        cout << "Error: " + WSAGetLastError() << endl;
        return FALSE;
    }
    return TRUE;
}

bool TcpSocket::listen(int backlog)
{
    if(::listen(socket_, backlog) == SOCKET_ERROR)
    {
        cout << "Error: " + WSAGetLastError() << endl;
        return FALSE;
    }
    return TRUE;
}

bool TcpSocket::accept(sockaddr_in* client_addr)
{
    
    int client_addr_len = sizeof(*client_addr);
    socket_srv = ::accept(socket_, (SOCKADDR *)client_addr, &client_addr_len);
    if(socket_srv != SOCKET_ERROR) return true;
    else{
        cout << "Error: " + WSAGetLastError() << endl;
        ::closesocket(socket_srv);
        return false;
    }
        
}

bool TcpSocket::close()
{
    ::closesocket(socket_);
    return TRUE;
}