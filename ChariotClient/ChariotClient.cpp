
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>

#pragma comment(lib,"Ws2_32.lib")

#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

void receive_messages(int client_socket) {
	char buffer[BUFFER_SIZE];
	while (true) {
		memset(buffer, 0, BUFFER_SIZE);
		int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

		if (bytes_received > 0) {
			std::cout << buffer;
		}
		else if (bytes_received == 0) {
			std::cout << "Connection closed by server." << std::endl;
			break;
		}
		else {
			std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
			break;
		}
	}
	closesocket(client_socket);
}

int main() {
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		std::cerr << "WSAStartup failed!" << std::endl;
		return 1;
	}

	int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

	if (connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		std::cerr << "Failed to connect to server: " << WSAGetLastError() << std::endl;
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	std::cout << "Conneted to the chat server!" << std::endl;

	std::thread receiver_thread(receive_messages, client_socket);
	receiver_thread.detach();

	std::string line;
	while (true) {
		std::getline(std::cin, line);

		if (line.empty()) continue;

		line += "\n";

		int bytes_sent = send(client_socket, line.c_str(), line.length(), 0);
		
		if (bytes_sent == SOCKET_ERROR) {
			std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
			break;
		}

		if (line == "/quit\n") {
			break;
		}
	}
	closesocket(client_socket);
	WSACleanup();
	
	return 0;
}

