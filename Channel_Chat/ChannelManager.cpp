#include"ChannelManager.h"
#include<algorithm>

ChannelManager& ChannelManager::getInstance() {
	static ChannelManager instance;
	return instance;
}

void ChannelManager::onClientConnected(int client_socket) {
	std::lock_guard<std::mutex> lock(mutex_);
	clients_[client_socket] = ClientInfo{ "","" };
}

void ChannelManager::onClientDisconnected(int client_socket) {
	leaveChannel(client_socket);
	std::lock_guard<std::mutex> lock(mutex_);
	clients_.erase(client_socket);
}

void ChannelManager::setClientNickname(int client_socket, const std::string& nickname) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (clients_.count(client_socket)) {
		clients_[client_socket].nickname = nickname;
	}
}

void ChannelManager::joinChannel(int client_socket, const std::string& channel_name) {
	leaveChannel(client_socket);

	std::lock_guard<std::mutex> lock(mutex_);
	if (clients_.count(client_socket)) {
		channels_[channel_name].push_back(client_socket);
		clients_[client_socket].current_channel = channel_name;
	}
}

void ChannelManager::leaveChannel(int client_socket) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (!clients_.count(client_socket)) return;

	std::string channel_name = clients_[client_socket].current_channel;
	if (!channel_name.empty() && channels_.count(channel_name)) {
		auto& members = channels_[channel_name];
		members.erase(std::remove(members.begin(), members.end(), client_socket), members.end());

		if (members.empty()) {
			channels_.erase(channel_name);
		}

		clients_[client_socket].current_channel = "";
	}
}

std::string ChannelManager::getLobbyInfo() {
	std::lock_guard<std::mutex> lock(mutex_);
	std::string info = "\n--- Lobby ---\nAvailable Channels:\n";
	if (channels_.empty()) {
		info += "  (No channels available yet)\n";
	}
	else {
		for (const auto& [name, members] : channels_) {
			info += "  - " + name + " (" + std::to_string(members.size()) + " users)\n";
		}
	}
	info += "----------------\n";
	return info;
}


ClientInfo ChannelManager::getClientInfo(int client_socket) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (clients_.count(client_socket)) {
		return clients_[client_socket];
	}
	return ClientInfo{};
}

std::vector<int> ChannelManager::getChannelMembers(const std::string& channel_name) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (channels_.count(channel_name)) {
		return channels_[channel_name];
	}
	return{};
}