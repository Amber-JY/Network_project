/*
discription:web服务器主函数
author：zjy
time：2020.10
*/

#include"Server.h"
#include<iostream>


int main(int argc, char* argv[])
{
    char temp;
    Server webserver(PORT,NULL);
    webserver.Run();
    webserver.closeserver();
    return 0;
}