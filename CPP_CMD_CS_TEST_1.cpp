#include <winsock2.h>                
#include <stdio.h>  
#include <string>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")  
using namespace std;

//数据结构体:用于数据交互
struct Data {
    uint16_t flags;     // 标志位:0x01登录,0x02退出,0x03发送信息
    uint16_t data_len;  // 负载长度(小于1000)
    char data[1020];    // 负载
};

//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    if (wstr) delete[] wstr;
    return str;
}

//GB2312到UTF-8的转换 
char* G2U(const char* gb2312,uint16_t &len)
{
    len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    if (wstr) delete[] wstr;
    return str;
}

int main()
{
    //SOCKET前的一些检查，检查协议库的版本，为了避免别的版本的socket，并且通过  
    //WSAStartup启动对应的版本，WSAStartup的参数一个是版本信息，一个是一些详细的细节，注意高低位  
    //WSAStartup与WSACleanup对应  
    int err;
    WORD versionRequired;
    WSADATA wsaData;
    versionRequired = MAKEWORD(1, 1);
    err = WSAStartup(versionRequired, &wsaData);//协议库的版本信息  

    //通过WSACleanup的返回值来确定socket协议是否启动  
    if (!err)
    {
        printf("客户端启动!\n");
    }
    else
    {
        printf("客户端启动失败!\n");
        return 0;//结束  
    }
    //创建socket这个关键词，这里想一下那个图形中的socket抽象层  
    //注意socket这个函数，他三个参数定义了socket的所处的系统，socket的类型，以及一些其他信息  
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET) {
        printf("创建Socket失败::%d\n", GetLastError());
        return -1;
    }

    //socket编程中，它定义了一个结构体SOCKADDR_IN来存计算机的一些信息，像socket的系统，  
    //端口号，ip地址等信息，这里存储的是服务器端的计算机的信息  
    SOCKADDR_IN clientsock_in;
    clientsock_in.sin_addr.S_un.S_addr = inet_addr("121.43.42.24");
    clientsock_in.sin_family = AF_INET;
    clientsock_in.sin_port = htons(1234);

    //前期定义了套接字，定义了服务器端的计算机的一些信息存储在clientsock_in中，  
    //准备工作完成后，然后开始将这个套接字链接到远程的计算机  
    //也就是第一次握手  

    int Ret = connect(clientSocket, (SOCKADDR*)&clientsock_in, sizeof(SOCKADDR));//开始连接  
    if (Ret == SOCKET_ERROR)
    {
        printf("连接失败::%d\n", GetLastError());
        return -1;
    }
    else
    {
        printf("连接服务器成功\n");
    }
    /*
    char buffer[40];
    printf("Please write:");
    scanf("%s", buffer);
    Ret = send(clientSocket, buffer, sizeof(buffer), 0);
    */
    Data send_data;
    send_data.flags = 0x01;
    Ret = send(clientSocket, (char*)&send_data,1024, 0);

    if (Ret == SOCKET_ERROR)
    {
        printf("LOGIN Error::%d\n", GetLastError());
    }

    
    while (1)
    {
        //char SendBuffer[100];
        //scanf("%s", SendBuffer);

        Data data;

        string buf;
        cin >> buf;
        sprintf(data.data,"%s", G2U(buf.c_str(), data.data_len));

        if ('q' == buf[0])
        {
            data.flags = 0x02;
            Ret = send(clientSocket, (char*)&data, 1024, 0);
            break;
        }
        data.flags = 0x03;
        /* send data to server */
        Ret = send(clientSocket,(char*)&data, 1024, 0);
        if (Ret == SOCKET_ERROR)
        {
            printf("Send Info Error::%d\n", GetLastError());
            break;
        }

        
    }


    /*
    char receiveBuf[100];

    //解释socket里面的内容  
    recv(clientSocket, receiveBuf, 101, 0);
    printf("%s\n", receiveBuf);

    //发送socket数据  
    send(clientSocket, "hello,this is client", strlen("hello,this is client") + 1, 0);
    */
    //关闭套接字  
    closesocket(clientSocket);
    //关闭服务  
    WSACleanup();
    return 0;
}