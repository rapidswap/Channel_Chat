#pragma once

class ClientHandler {
public:
	// �� ��ȯ ���� explicit
	explicit ClientHandler(int client_socket);

	void run();

private:
	int client_socket_;
};