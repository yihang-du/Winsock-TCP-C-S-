#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 5050
#define serAddr "127.0.0.1"
#define DATA_BUFFER 1024

int main() {
    WSADATA wsadata;
    SOCKET sClient;
    struct sockaddr_in ser;
    int iPort = DEFAULT_PORT;
    char SendBuf[DATA_BUFFER];
    char RecvBuf[DATA_BUFFER];

    printf("-----------\n客户端启动\n-------------\n\n");

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("fail to load winsock\n");
        return 0;
    }

    ser.sin_family = AF_INET;
    ser.sin_port = htons(iPort);
    InetPton(AF_INET, L"127.0.0.1", &ser.sin_addr);

    sClient = socket(AF_INET, SOCK_STREAM, 0);
    if (sClient == INVALID_SOCKET) {
        printf("socket() error: %d\n", WSAGetLastError());
        WSACleanup();
        return 0;
    }

    if (connect(sClient, (struct sockaddr*)&ser, sizeof(ser)) == SOCKET_ERROR) {
        printf("connect() error: %d\n", WSAGetLastError());
        closesocket(sClient);
        WSACleanup();
        return 0;
    }

    printf("输入一个十进制数: ");
    fgets(SendBuf, DATA_BUFFER, stdin);
    SendBuf[strcspn(SendBuf, "\n")] = '\0'; // 去掉换行符

    if (send(sClient, SendBuf, strlen(SendBuf) + 1, 0) == SOCKET_ERROR) {
        printf("send() error: %d\n", WSAGetLastError());
        closesocket(sClient);
        WSACleanup();
        return 0;
    }

    memset(RecvBuf, 0, sizeof(RecvBuf));
    if (recv(sClient, RecvBuf, sizeof(RecvBuf), 0) == SOCKET_ERROR) {
        printf("recv() error: %d\n", WSAGetLastError());
        closesocket(sClient);
        WSACleanup();
        return 0;
    }
    printf("收到服务器的二进制消息: %s\n", RecvBuf);

    closesocket(sClient);
    WSACleanup();

    return 0;
}
