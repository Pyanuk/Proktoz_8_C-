#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <Winsock2.h>
#include <WS2tcpip.h>



int main()
{

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;
    }


    ADDRINFO hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    ADDRINFO* addrResult = nullptr;
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        std::cerr << "getaddrinfo failed with error: " << result << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }


    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(addrResult);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult);

    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for client connection..." << std::endl;


    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket);

    char recvBuffer[512];
    const char* sendBuffer = "Hello from Russian";
    do {
        ZeroMemory(recvBuffer, sizeof(recvBuffer));
        result = recv(ClientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (result > 0) {
            std::cout << "Received " << result << " bytes: " << recvBuffer << std::endl;
        }
        else if (result == 0) {
            std::cout << "Connection closed by client" << std::endl;
        }
        else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (result > 0);

    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}