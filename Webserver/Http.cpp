/*
 * @Author: your name
 * @Date: 2020-10-10 13:37:17
 * @LastEditTime: 2020-12-10 11:46:42
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \MyNet\Http.cpp
 */
#include"Http.h"




int Http::Init()
{
    Mimetype["html"] = "text/html";
    Mimetype["jpg"] = "image/jpeg";
    Mimetype["ico"] = "image/x-icon";
    return 0;
}

int Http::Error(SOCKET& socket)
{
    char error_head[1024];
    int error_html_len = sizeof(http_res_hdr_tmp2);
    int error_len = sprintf(error_head, http_res_hdr_tmp2, error_html_len);
    send(socket, error_head, error_len, 0);
    return 0;
}

int Http::Analy_http(char* recv_buf)
{
    //得到url
    char *head, *end, *bias;
    head = strchr(recv_buf, ' ') + 1;
    end = strchr(head, ' ');
    *end = 0;
    bias = strrchr(head, '/');
    int length = end - bias;
    
    if (*bias == '/'){
		bias++;
		length--;
	}

	memcpy(filename, bias, length);
    
    filename[length] = 0;
	head = strchr(filename, '.');
    char type_[10];
    if (head)
		strcpy(type_, head + 1);
    type = Mimetype[type_];

    return 0;
}

int Http::Send_file()
{
    file = fopen(filename, "rb+");
    if(file == NULL){
        cout << endl << "File lost." << endl;
        return -1;
	}
    if(type.empty()){
        cout << "Type error." << endl;
        return -1;
    }
    fseek(file, 0, SEEK_END);                      // 设置指针到文件流尾部
    file_len = ftell(file);                        // 指针距离文件头部的距离，即为文件流大小
    fseek(file, 0,SEEK_SET);                              // 指针重回文件流头部
    return 0;
}