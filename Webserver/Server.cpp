/*
discription:web服务器主体，继承自TcpSocket
author:zjy
time:2020.10
*/
#include"Server.h"

Server::Server(){
    http.Init();
}
Server::~Server() {
    cout <<endl << "Server close." << endl;
}

void Check(Server* server)
{
    while (_kbhit() <= 0)
        ;
    server->~Server();
    exit(0);
}

Server::Server( unsigned short int port , const char *ip )
{
    WSADATA wsaData;	
	WORD sockVersion = MAKEWORD(2, 2);
	if(WSAStartup(sockVersion, &wsaData) != 0){
        cout << "The web server init failed." << endl;
    }
    cout << "The web server init succesed." << endl;
    http.Init();
    
    if (!create()) {
        cout << "socket create error." << endl;
    }
    cout << "create success" <<endl;
    if (!bind()) {
        cout << "socket bind error." << endl;
    }
    cout << "bind success." << endl;
    if (!listen(5)) {
        cout << "socket listen error." << endl;
    }
    cout << "listen success." << endl;

    this->endflag=false;
    cout << "webserver initial success." << endl;
};


bool Server::recv()
{
    recv_len = ::recv(socket_srv, recv_buf, BUFSIZE, 0);
    if( recv_len > 0 ) return true;
    else
        false;
}

bool Server::closeserver(){
    if(!close()){
        cout << "close error." << endl;
        return false;
    };
    WSACleanup();
    return true;
}

int Server::Run()
{
    int round = 1;
    std::thread newThread(Check, this);
    newThread.detach();
    while(1){
        cout << "wait for connecting." << endl;
        
        bool result = accept(&addrClient);
        if(socket_srv == INVALID_SOCKET) {
            cout << "Failed to accept socket." << endl;
			closesocket(socket_srv);
            continue;
        }
        cout << "accept success." << endl;
        printf("|The server accpet from IP: %s, port: %d|\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));		
        if(!recv()){
            cout << "recive error." << endl;
            closesocket(socket_srv);
            continue;
        }
        cout << "Recive sucess:" << endl;
        cout << endl << "---------------------round"<<round<<"----------------------" << endl;
        cout << recv_buf;
        cout << endl << "-----------------------------------------------------" ;

        http.Analy_http(recv_buf);
        if (http.Send_file() == -1) {
            cout << "send error. Enter q to quit." << endl;
            http.Error(socket_srv);
            closesocket(socket_srv);
            if ((cin.get()) == 'q')
                return -1;
            continue;
        }

        char http_header[1024];
        int hdr_len = sprintf(http_header, http.http_res_hdr_tmp1, http.file_len, http.type.c_str());
        if (send(socket_srv, http_header, hdr_len, 0) == SOCKET_ERROR) {
            cout << "Error: " << WSAGetLastError() << endl;
            http.Error(socket_srv);
            closesocket(socket_srv);
            continue;
        }

        int read_len, send_len;
        do {
            read_len = fread(send_buf, sizeof(char), 1024, http.file);
            if (read_len > 0) {
                if(send_len = send(socket_srv, send_buf, read_len, 0)==SOCKET_ERROR)
                    http.Error(socket_srv);
                http.file_len -= read_len;
            }
        } while ((read_len > 0) & (http.file_len > 0));
        cout <<endl<< "require " << round << " resolved." << endl;
        fclose(http.file);
        round++;
    };
    fclose(http.file);
    return 0;
}