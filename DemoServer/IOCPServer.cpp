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

// 지금 클라이언트 매니저에서 클라이언트의 연결을 관리하려고 하는 중인데, 어차피 클라이언트(세션)은 로그인, 게임 진행 중에도 지속적으로 접근을 해서 값을 변경시켜야 하므로
// 어떻게 캡슐화를 이어나가야할지에 대해 뇌절이 온 상태\
// clients 배열을 clientmanager 클래스에 놓아 봤자 계속적인 접근이 이루어져야 하므로 배열 위치를 옮겨야하나 고민도 되고.. 어떻게 캡슐화를 하지..?
void IOCPServer::ProcessGQCS(OP_TYPE _op_type)
{
	switch (_op_type) { 
						
	case OP_ACCEPT: {
		int client_id = GetClientId();
		if (client_id != -1) {
			clients[client_id].b_use = true;
			clients[client_id].m_player.m_id = client_id;
			clients[client_id].m_player.portal.m_id = client_id;

			// 이제 좌표 할당을 room에 클라이언트가 할당될 때 room 내부의 배열 위치를 기준으로 할당할 예정

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
			// 테스트용 시작 좌표 
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
			AddClientInRoom(&clients[client_id]); // 어차피 전체 클라이언트 수와 방의 크기 수를 맞춰놔서 일단 패스
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
//		clients[client_id].m_player.portal.m_id = client_id;  // 얘는 게임 시작할 때 할당해주는 것이 맞는 것 같기는 한데..
//
//		clients[client_id].m_prev_remain = 0;
//		clients[client_id].m_socket = c_socket;
//
//		return client_id; // 핸들에 소켓을 등록할 때 id가 필요하므로 어쩔 수 없이 반환한다.
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
	std::cout << "클라이언트 " << c_id << " 연결 끊김" << std::endl;

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
	// 이거 구현해야 한다. 근데 레퍼런스로 받아서 그대로 레퍼런스로 넣으면 현재 룸의 해당 배열의 클라이언트 자리가 사용되고 있는지 알 수가 없다. 따라서 기존에 클라이언트 배열에 
	// 새 세션을 추가할 때처럼 부울 변수 하나와 뮤텍스를 추가로 두고 해당 배열 자리가 사용되고 있는지 아닌지 관리가 필요하다. 기존 세션 클래스+추가 변수로 이루어진 구조체가 필요할 듯 하다.
	// 혹은 레퍼런스가 아닌 포인터를 사용하여 null 여부로 판정하는 것도 가능할 듯 하다.->이거 사용해서 빠르게 해봐야겠다.

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
