#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 5050
#define serAddr "127.0.0.1"
#define DATA_BUFFER 1024

void DecimalToBinary(int n, char* binaryStr) {
    int index = 0;
    for (int i = 31; i >= 0; i--) {
        int k = n >> i;
        if (k & 1) {
            binaryStr[index++] = '1';
        }
        else {
            binaryStr[index++] = '0';
        }
    }
    binaryStr[index] = '\0';
}

int main() {
    WSADATA wsadata;
    SOCKET sListen, sAccept;
    struct sockaddr_in ser, cli;
    int iPort = DEFAULT_PORT;
    int iLen = sizeof(cli);
    int iSend, iRecv;
    char RecvBuf[DATA_BUFFER];
    char SendBuf[DATA_BUFFER];
    wchar_t cliIP[INET_ADDRSTRLEN];

    printf("-----------\n服务器运行中，等待客户登录\n-------------\n\n");

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("fail to load winsock");
        return 0;
    }

    ser.sin_family = AF_INET;
    ser.sin_port = htons(iPort);
    InetPton(AF_INET, L"127.0.0.1", &ser.sin_addr);

    sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (sListen == INVALID_SOCKET) {
        printf("socket() error:%d\n", WSAGetLastError());
        return 0;
    }

    if (bind(sListen, (struct sockaddr*)&ser, sizeof(ser)) == SOCKET_ERROR) {
        printf("bind() error:%d\n", WSAGetLastError());
        return 0;
    }

    if (listen(sListen, 5) == SOCKET_ERROR) {
        printf("Listen() error:%d\n", WSAGetLastError());
        return 0;
    }

    while (true) {
        sAccept = accept(sListen, (struct sockaddr*)&cli, &iLen);
        if (sAccept == INVALID_SOCKET) {
            printf("accept() error:%d\n", WSAGetLastError());
            return 0;
        }
        else {
            InetNtop(AF_INET, &cli.sin_addr, cliIP, INET_ADDRSTRLEN);
            wprintf(L"客户机(IP:%s,port:%d)连接成功!\n\n", cliIP, ntohs(cli.sin_port));
        }

        memset(RecvBuf, 0, sizeof(RecvBuf));
        iRecv = recv(sAccept, RecvBuf, sizeof(RecvBuf), 0);
        if (iRecv == 0) {
            return 0;
        }
        else if (iRecv == SOCKET_ERROR) {
            printf("recv() error:%d\n", WSAGetLastError());
            return 0;
        }
        printf("收到客户机的消息:%s\n", RecvBuf);

        int decimalNumber = atoi(RecvBuf); // 将收到的字符串转换为整数
        DecimalToBinary(decimalNumber, SendBuf); // 将整数转换为二进制字符串
        printf("向客户机发送二进制信息:%s\n", SendBuf);

        iSend = send(sAccept, SendBuf, strlen(SendBuf) + 1, 0);
        if (iSend == SOCKET_ERROR) {
            printf("send() error:%d\n", WSAGetLastError());
            return 0;
        }
        else if (iSend == 0) {
            break;
        }
        else {
            printf("--------------------\n\n");
        }
        closesocket(sAccept);
    }

    closesocket(sListen);
    WSACleanup();
    return 0;
}
