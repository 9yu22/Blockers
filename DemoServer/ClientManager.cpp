#include<iostream>
#include <mutex>
#include "ClientManager.h"

int ClientManager::LoginClient(SOCKET c_socket)
{
	int client_id = GetClientId();
	if (client_id != -1) {
		clients[client_id].b_use = true;
		clients[client_id].m_player.m_id = client_id;
		clients[client_id].m_player.portal.m_id = client_id;  // ��� ���� ������ �� �Ҵ����ִ� ���� �´� �� ����� �ѵ�..

		clients[client_id].m_prev_remain = 0;
		clients[client_id].m_socket = c_socket;

		return client_id; // �ڵ鿡 ������ ����� �� id�� �ʿ��ϹǷ� ��¿ �� ���� ��ȯ�Ѵ�.
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
	std::cout << "Ŭ���̾�Ʈ " << c_id << " ���� ����" << std::endl;

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
