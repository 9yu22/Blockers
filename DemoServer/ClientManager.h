#pragma once
#include <array>
#include "Session.h"
#include "protocol.h"

constexpr int MAX_USER = 4;
class ClientManager
{
	std::array<Session, MAX_USER> clients;

public:
	int LoginClient(SOCKET c_socket);
	void DisconnectClient(int c_id);
	int GetClientId();
};

