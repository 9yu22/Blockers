#include<iostream>
#include <mutex>
#include "ClientManager.h"

int ClientManager::LoginClient(SOCKET c_socket)
{
	int client_id = GetClientId();
	if (client_id != -1) {
		clients[client_id].b_use = true;
		clients[client_id].m_player.m_id = client_id;
		clients[client_id].m_player.portal.m_id = client_id;  // 얘는 게임 시작할 때 할당해주는 것이 맞는 것 같기는 한데..

		clients[client_id].m_prev_remain = 0;
		clients[client_id].m_socket = c_socket;

		return client_id; // 핸들에 소켓을 등록할 때 id가 필요하므로 어쩔 수 없이 반환한다.
	}

	else {
		return -1;
	}
}

void ClientManager::DisconnectClient(int c_id)
{
	for (auto& pl : clients) {
		{
			std::lock_guard<std::mutex> mx(clients[c_id].session_mx);
			if (true != pl.b_use) continue;
		}
		pl.send_remove_player_packet(clients[c_id].m_player);
	}
	closesocket(clients[c_id].m_socket);
	std::cout << "클라이언트 " << c_id << " 연결 끊김" << std::endl;

	{
		std::lock_guard<std::mutex> mx(clients[c_id].session_mx);
		clients[c_id].b_use = false;
	}
}

int ClientManager::GetClientId()
{
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard<std::mutex> assign_id{ clients[i].session_mx };
		if (clients[i].b_use == false)
			return i;
	}

	return -1;
}
