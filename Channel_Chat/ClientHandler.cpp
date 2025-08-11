#include "ClientHandler.h"
#include "ChannelManager.h"
#include <iostream>
#include <string>
#include <vector>
#include <WinSock2.h>

#define BUFFER_SIZE 1024

ClientHandler::ClientHandler(int client_socket) : client_socket_(client_socket) {}

void ClientHandler::run() {
	auto& manager = ChannelManager::getInstance();
	manager.onClientConnected(client_socket_);

	char buffer[BUFFER_SIZE];

	std::string welcome_prompt = "Enter your nickname: ";
	send(client_socket_, welcome_prompt.c_str(), welcome_prompt.length(), 0);

	memset(buffer, 0, BUFFER_SIZE);
	int bytes_received = recv(client_socket_, buffer, BUFFER_SIZE, 0);
	if (bytes_received <= 0) {
		std::cerr << "Client disconnected before setting a nickname." << std::endl;
		manager.onClientDisconnected(client_socket_);
		closesocket(client_socket_);
		return;
	}

	std::string nickname(buffer, strcspn(buffer, "\r\n")); // telnet 개행문자(CRLF)
	manager.setClientNickname(client_socket_, nickname);

	std::string welcome_msg = "Welcome, " + nickname + "!\n";
	std::string lobby_info = manager.getLobbyInfo();
	std::string full_welcome = welcome_msg + lobby_info + "Commands: /join [channel], /exit \n";
	send(client_socket_, full_welcome.c_str(), full_welcome.length(), 0);

	while (true) {
		memset(buffer, 0, BUFFER_SIZE);
		bytes_received = recv(client_socket_, buffer, BUFFER_SIZE, 0);

		if (bytes_received <= 0) {
			std::cerr << nickname << " has disconnected." << std::endl;
			break;
		}

		std::cout << "DEBUG_HEX: ";
		for (int i = 0; i < bytes_received; ++i) {
			printf("%02x ", (unsigned char)buffer[i]);
		}
		std::cout << std::endl;

		std::string message(buffer, strcspn(buffer, "\r\n"));

		std::cout << "DEBUG_MSG: [" << message << "]" << std::endl;


		if (message.rfind("/join ", 0) == 0) {
			std::string channel_to_join = message.substr(6);

			std::string old_channel = manager.getClientInfo(client_socket_).current_channel;
			if (!old_channel.empty()) {
				std::string leave_broadcast_msg = "[" + old_channel + "] " + nickname + " has left. \n";
				auto members = manager.getChannelMembers(old_channel);
				for (int member_socket : members) {
					if (member_socket != client_socket_) {
						send(member_socket, leave_broadcast_msg.c_str(), leave_broadcast_msg.length(), 0);
					}
				}
			}

			manager.joinChannel(client_socket_, channel_to_join);
			std::string join_broadcast_msg = "[" + channel_to_join + "] " + nickname + " has joined. \n";
			auto members = manager.getChannelMembers(channel_to_join);
			for (int member_socket : members) {
				send(member_socket, join_broadcast_msg.c_str(), join_broadcast_msg.length(), 0);
			}
		}
		else if (message == "/exit") {
			std::string current_channel = manager.getClientInfo(client_socket_).current_channel;
			if (!current_channel.empty()) {
				std::string leave_broadcast_msg = "[" + current_channel + "] " + nickname + " has left. \n";
				manager.leaveChannel(client_socket_);
				auto members = manager.getChannelMembers(current_channel);
				for (int member_socket : members) {
					send(member_socket, leave_broadcast_msg.c_str(), leave_broadcast_msg.length(), 0);
				}
				std::string exit_feedback = "You have left the channel.\n" + manager.getLobbyInfo();
				send(client_socket_, exit_feedback.c_str(), exit_feedback.length(), 0);
			}
		}
		else if (message == "/refresh") {
			std::string refresh_feedback = "You refrash mainform.\n" + manager.getLobbyInfo();
			send(client_socket_, refresh_feedback.c_str(), refresh_feedback.length(), 0);
		}
		else {
			ClientInfo info = manager.getClientInfo(client_socket_);
			if (!info.current_channel.empty()) {
				std::string chat_msg = "[" + info.nickname + "]: " + message + "\n";
				auto members = manager.getChannelMembers(info.current_channel);
				for (int member_socket : members) {
					if (member_socket != client_socket_) {
						send(member_socket, chat_msg.c_str(), chat_msg.length(), 0);
					}
				}
			}
			else {
				std::string no_channel_msg = "You are not in a channel. Use /join [channel] to talk.\n";
				send(client_socket_, no_channel_msg.c_str(), no_channel_msg.length(), 0);
			}
		}
	}

	manager.onClientDisconnected(client_socket_);
	closesocket(client_socket_);
}