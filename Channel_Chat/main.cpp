#include <iostream>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include "ClientHandler.h"

#pragma comment(lib,"Ws2_32.lib")
#define PORT 12345

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup falied!" << std::endl;
		return 1;
	}
	
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	if (bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		std::cerr<<"Bind failed with error: "
	}

	return 0;
}