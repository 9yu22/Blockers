#include <iostream>
#include "IOCPServer.h"
#include "protocol.h"

IOCPServer::IOCPServer()
{
}

IOCPServer::~IOCPServer()
{
}

void IOCPServer::StartServer()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(s_socket), h_iocp, 9999, 0);
	c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	ac_over._op_type = OP_ACCEPT;
	AcceptEx(s_socket, c_socket, ac_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &ac_over._over);
}

// ���� Ŭ���̾�Ʈ �Ŵ������� Ŭ���̾�Ʈ�� ������ �����Ϸ��� �ϴ� ���ε�, ������ Ŭ���̾�Ʈ(����)�� �α���, ���� ���� �߿��� ���������� ������ �ؼ� ���� ������Ѿ� �ϹǷ�
// ��� ĸ��ȭ�� �̾���������� ���� ������ �� ����\
// clients �迭�� clientmanager Ŭ������ ���� ���� ������� ������ �̷������ �ϹǷ� �迭 ��ġ�� �Űܾ��ϳ� ��ε� �ǰ�.. ��� ĸ��ȭ�� ����..?
void IOCPServer::ProcessGQCS(OP_TYPE _op_type)
{
	switch (_op_type) { 
						
	case OP_ACCEPT: {
		int client_id = GetClientId();
		if (client_id != -1) {
			clients[client_id].b_use = true;
			clients[client_id].m_player.m_id = client_id;
			clients[client_id].m_player.portal.m_id = client_id;

			// ���� ��ǥ �Ҵ��� room�� Ŭ���̾�Ʈ�� �Ҵ�� �� room ������ �迭 ��ġ�� �������� �Ҵ��� ����

			//switch (client_id % 4) {
			//case 0:
			//	clients[client_id].m_player.SetWorldLocation(7000.f, 7000.f, 1100.f);
			//	clients[client_id].m_player.SetRespawnLocation(7000.f, 7000.f, 1100.f);
			//	clients[client_id].m_player.portal.SetWorldLocation(6500.f, 6500.f, 300.f);
			//	break;
			//case 1:
			//	clients[client_id].m_player.SetWorldLocation(-7000.f, 7000.f, 1100.f);
			//	clients[client_id].m_player.SetRespawnLocation(-7000.f, 7000.f, 1100.f);
			//	clients[client_id].m_player.portal.SetWorldLocation(-6500.f, 6500.f, 300.f);
			//	break;
			//case 2:
			//	clients[client_id].m_player.SetWorldLocation(7000.f, -7000.f, 1100.f);
			//	clients[client_id].m_player.SetRespawnLocation(7000.f, -7000.f, 1100.f);
			//	clients[client_id].m_player.portal.SetWorldLocation(6500.f, -6500.f, 300.f);
			//	break;
			//case 3:
			//	clients[client_id].m_player.SetWorldLocation(-7000.f, -7000.f, 1100.f);
			//	clients[client_id].m_player.SetRespawnLocation(-7000.f, -7000.f, 1100.f);
			//	clients[client_id].m_player.portal.SetWorldLocation(-6500.f, -6500.f, 300.f);
			//	break;
			//}
			//
			// �׽�Ʈ�� ���� ��ǥ 
			//switch (client_id % 4) {
			//case 0:
			//	clients[client_id].m_player.SetWorldLocation(700.f, 700.f, 1000.f);
			//	clients[client_id].m_player.SetRespawnLocation(700.f, 700.f, 1000.f);
			//	clients[client_id].m_player.portal.SetWorldLocation(950.f, 950.f, 700.f);
			//	break;
			//case 1:
			//	clients[client_id].m_player.SetWorldLocation(-700.f, -700.f, 1000.f);
			//	clients[client_id].m_player.SetRespawnLocation(-700.f, -700.f, 1000.f);
			//	clients[client_id].m_player.portal.SetWorldLocation(-950.f, -950.f, 700.f);
			//	break;
			//case 2:
			//	clients[client_id].m_player.SetWorldLocation(700.f, -700.f, 1000.f);
			//	clients[client_id].m_player.SetRespawnLocation(700.f, -700.f, 1000.f);
			//	clients[client_id].m_player.portal.SetWorldLocation(950.f, -950.f, 700.f);
			//	break;
			//case 3:
			//	clients[client_id].m_player.SetWorldLocation(-700.f, 700.f, 1000.f);
			//	clients[client_id].m_player.SetRespawnLocation(-700.f, 700.f, 1000.f);
			//	clients[client_id].m_player.portal.SetWorldLocation(-950.f, 950.f, 700.f);
			//	break;
			//}

			clients[client_id].m_prev_remain = 0;
			clients[client_id].m_socket = c_socket;
			AddClientInRoom(&clients[client_id]); // ������ ��ü Ŭ���̾�Ʈ ���� ���� ũ�� ���� ������� �ϴ� �н�
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket),
				h_iocp, client_id, 0);
			clients[client_id].do_recv();
		}
		else {
			std::cout << "Max user exceeded.\n";
			closesocket(c_socket);
		}
		c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		ZeroMemory(&ac_over._over, sizeof(ac_over._over));
		int addr_size = sizeof(SOCKADDR_IN);
		AcceptEx(s_socket, c_socket, ac_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &ac_over._over);
		break;
	}
	case OP_RECV: {
		int remain_data = num_bytes + clients[key].m_prev_remain;
		char* p = ex_over->_send_buf;
		while (remain_data > 0) {
			int packet_size = p[0];
			if (packet_size <= remain_data) {
				int num = clients[static_cast<int>(key)].room_num;
				rooms[num].ProcessPacket(static_cast<int>(key), p);
				p = p + packet_size;
				remain_data = remain_data - packet_size;
			}
			else break;
		}
		clients[key].m_prev_remain = remain_data;
		if (remain_data > 0)
			memcpy(ex_over->_send_buf, p, remain_data);
		clients[key].do_recv();
		break;
	}
	case OP_SEND:
		//if (ex_over->_packet_type == SC_LOGIN_INFO) {
		//	std::cout << "GQCS Login Send" << std::endl;
		//}
		//	
		//else if (ex_over->_packet_type == SC_ADD_PLAYER) {
		//	std::cout << "GQCS Add Send" << std::endl;
		//}

		/*else if (ex_over->_packet_type == SC_MOVE_PLAYER)
			std::cout << "GQCS Move Send" << std::endl;*/

		delete ex_over;
		break;
	}
}

//int IOCPServer::LoginClient(SOCKET c_socket)
//{
//	int client_id = GetClientId();
//	if (client_id != -1) {
//		clients[client_id].b_use = true;
//		clients[client_id].m_player.m_id = client_id;
//		clients[client_id].m_player.portal.m_id = client_id;  // ��� ���� ������ �� �Ҵ����ִ� ���� �´� �� ����� �ѵ�..
//
//		clients[client_id].m_prev_remain = 0;
//		clients[client_id].m_socket = c_socket;
//
//		return client_id; // �ڵ鿡 ������ ����� �� id�� �ʿ��ϹǷ� ��¿ �� ���� ��ȯ�Ѵ�.
//	}
//
//	else {
//		return -1;
//	}
//}

void IOCPServer::DisconnectClient(int c_id)
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

int IOCPServer::GetClientId()
{
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard<std::mutex> assign_id{ clients[i].session_mx };
		if (clients[i].b_use == false)
			return i;
	}

	return -1;
}

bool IOCPServer::AddClientInRoom(Session* s)
{
	// �̰� �����ؾ� �Ѵ�. �ٵ� ���۷����� �޾Ƽ� �״�� ���۷����� ������ ���� ���� �ش� �迭�� Ŭ���̾�Ʈ �ڸ��� ���ǰ� �ִ��� �� ���� ����. ���� ������ Ŭ���̾�Ʈ �迭�� 
	// �� ������ �߰��� ��ó�� �ο� ���� �ϳ��� ���ؽ��� �߰��� �ΰ� �ش� �迭 �ڸ��� ���ǰ� �ִ��� �ƴ��� ������ �ʿ��ϴ�. ���� ���� Ŭ����+�߰� ������ �̷���� ����ü�� �ʿ��� �� �ϴ�.
	// Ȥ�� ���۷����� �ƴ� �����͸� ����Ͽ� null ���η� �����ϴ� �͵� ������ �� �ϴ�.->�̰� ����ؼ� ������ �غ��߰ڴ�.

	for (auto& room : rooms) {
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (room.clients[i] != nullptr) {
				room.clients[i] = s;
				room.SetPlayerInfo(i);
				return true;
			}
		}
	}
	return false;
}

void IOCPServer::Run()
{
	while (true) {
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		ex_over = reinterpret_cast<EX_OVERLAPPED*>(over);
		if (FALSE == ret) {
			if (ex_over->_op_type == OP_ACCEPT) {
				std::cout << "Accept Error";
				exit(-1);
			}
			else {
				std::cout << "GQCS Error on client[" << key << "]\n";
				DisconnectClient(static_cast<int>(key));
				if (ex_over->_op_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		ProcessGQCS(ex_over->_op_type);
	}
}

void IOCPServer::EndServer()
{
	closesocket(s_socket);
	WSACleanup();
}
