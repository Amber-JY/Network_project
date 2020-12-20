/*
discribtion:web服务器配置文件
author:zjy
time:2020.10
*/
#include<string>
#include<winsock2.h>
#pragma comment(lib, "ws2_32")

#define BUFSIZE 1024

using namespace std;

static int BACKLOG = 5;
static long SRVADDRESS = INADDR_ANY;
static int PORT = 80;

