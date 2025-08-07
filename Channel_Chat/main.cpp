#include <iostream>
#include "ChannelManager.h"

#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup falied!" << std::endl;
		return 1;
	}
	std::cout << "WSAStartup Success!" << std::endl;
	
	std::cout << "Getting ChannelManager instance..." << std::endl;
	ChannelManager& manager = ChannelManager::getInstance();

	std::cout << "시뮬레이션 클라이언트 연결..." << std::endl;
	manager.onClientConnected(1001);
	manager.setClientNickname(1001, "userA");
	manager.joinChannel(1001, "general");

	manager.onClientConnected(1002);
	manager.setClientNickname(1002, "userB");
	manager.joinChannel(1002, "general");

	manager.onClientConnected(1003);
	manager.setClientNickname(1003, "userB");
	manager.joinChannel(1003, "general");

	std::cout << manager.getLobbyInfo() << std::endl;

	std::cout << "시뮬레이션 클라이언트 접속 종료..." << std::endl;
	manager.onClientDisconnected(1002);

	std::cout << manager.getLobbyInfo() << std::endl;

	WSACleanup();
	std::cout << "WSACleanup Success!" << std::endl;

	return 0;
}