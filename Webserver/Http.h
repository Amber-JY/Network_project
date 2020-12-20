#include<map>
#include<string>
#include<cstring>
#include<iostream>
#include<stdio.h>
#include<fstream>
#include<winsock2.h>

using namespace std;

typedef map<string, string> FILETYPE;

class Http{

private:
    char url[20];

public:
    int file_len;
    FILE *file;
    string type;
    FILETYPE Mimetype;
    char filename[30];
    const char *http_res_hdr_tmp1 = "HTTP/1.1 200 OK \r\nServer:Win10<0.1>\r\n"
                                    "Accept-Ranges:bytes\r\nContent-Length:%d\r\nConnection:close\r\n"
                                    "Content-Type:%s\r\n\r\n";
    const char* http_res_hdr_tmp2 = "HTTP/1.1 404 Not Found \r\n"
        "Content-Length:%d\r\nConnection:close\r\n"
        "Content-Type:text/html\r\n";
    
    //初始化文件库
    int Init();
    int Error(SOCKET& socket);
    int Analy_http(char *recv_buf);
    int Send_file();
};

