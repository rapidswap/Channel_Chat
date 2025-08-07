#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>

struct ClientInfo {
	std::string nickname;
	std::string current_channel;
};

class ChannelManager {
public:
	static ChannelManager& getInstance();

	ChannelManager(const ChannelManager&) = delete;
	void operator=(const ChannelManager&) = delete;

	void onClientConnected(int client_socket);
	void onClientDisconnected(int client_socket);
	void setClientNickname(int client_socket, const std::string& nickname);
	void joinChannel(int client_socket, const std::string& channel_name);
	void leaveChannel(int client_socket);
	std::string getLobbyInfo();
	ClientInfo getClientInfo(int client_socket);

	std::vector<int> getChannelMembers(const std::string& channel_name);

private:
	ChannelManager() {}
	~ChannelManager() {}

	std::map<int, ClientInfo>clients_;
	std::map<std::string, std::vector<int>> channels_;
	std::mutex mutex_;

};