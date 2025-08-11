#pragma once

class ClientHandler {
public:
	// 형 변환 막기 explicit
	explicit ClientHandler(int client_socket);

	void run();

private:
	int client_socket_;
};