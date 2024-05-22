#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <Winsock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO* addrResult;
    const char* sendBuffer = "Hello from Russian";
    char recvBuffer[512];

    SOCKET ConnectSocket = INVALID_SOCKET;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed" << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo("127.0.0.1", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed" << endl;
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "socket failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "connect failed" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server" << endl;

    do {
        result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
        if (result == SOCKET_ERROR) {
            cout << "send failed error" << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        result = recv(ConnectSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (result > 0) {
            recvBuffer[result] = '\0';
            cout << "Received " << result << " bytes: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed by server" << endl;
        }
        else {
            cout << "recv failed with error" << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

    } while (result > 0);

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
