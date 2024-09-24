#include <iostream>
#include <array>
//#include <WS2tcpip.h>
//#include <MSWSock.h>
#include "Session.h"
#include "protocol.h"
#include "Map.h"
#include "ItemSpawner.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
constexpr int MAX_USER = 5000;

SOCKET c_socket, server;
EX_OVERLAPPED a_over;

array<Session, MAX_USER> clients;
Map map;

int set_item_id = 0;
bool is_timer = false;

void worker_thread(HANDLE h_iocp);

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard<mutex> assign_id{ clients[i].session_mx };
		if (clients[i].b_use == false)
			return i;
	}
		
	return -1;
}

void disconnect(int c_id)
{
	for (auto& pl : clients) {
		{
			lock_guard<mutex> mx(clients[c_id].session_mx);
			if (true != pl.b_use) continue;
		}
		pl.send_remove_player_packet(clients[c_id].m_player);		
	}
	closesocket(clients[c_id].m_socket);
	std::cout << "클라이언트 " << c_id << " 연결 끊김" << std::endl;

	{
		lock_guard<mutex> mx(clients[c_id].session_mx);
		clients[c_id].b_use = false;
	}
	
}

void process_packet(int c_id, char* packet)
{
	static CS_ADD_BLOCK_PACKET prev_add;
	static CS_REMOVE_BLOCK_PACKET prev_remove;

	//if (!is_timer) {
	//	item_manager.InitStartTime();
	//	is_timer = true;
	//}

	// 일단 임시로 여기에..
	//item_manager.CalculateItemSpawnTime(); // 시간 계산
	//Item_type type = item_manager.CheckCanSpawn(); // 스폰 시간이 된 아이템을 반환

	//if (type != NONE) { // 모든 클라에게 해당 아이템(4개 위치) 전송
	//	for (int i = 0; i < 4; i++) {
	//		Item item(++set_item_id, type, i);

	//		for (auto& pl : clients) {
	//			if (false == pl.b_use) continue;
	//			pl.send_add_item_packet(item);
	//			//switch (item.item_type) {
	//			//case STONE:
	//			//	std::cout << "클라이언트 " <<pl.m_player.m_id << "에게 돌 전송" << std::endl;
	//			//	break;

	//			//case AMETHYST:
	//			//	std::cout << "클라이언트 " << pl.m_player.m_id << "에게 자수정 전송" << std::endl;
	//			//	break;

	//			//case RUBY:
	//			//	std::cout << "클라이언트 " << pl.m_player.m_id << "에게 루비 전송" << std::endl;
	//			//	break;

	//			//case DIAMOND:
	//			//	std::cout << "클라이언트 " << pl.m_player.m_id << "에게 다이아 전송" << std::endl;
	//			//	break;
	//			//}
	//		}
	//	}
	//}

	//for (int i = 0; i < 4; i++)
	//{
	//	std::cout << "calculate_spawn_time: " << item_manager.calculate_spawn_time[i] << std::endl;
	//}
	//std::cout << std::endl;	
	//std::cout << "calculate_time_for_int: " << item_manager.calculateTimeForInt << std::endl;

	switch (packet[1]) {
	case CS_LOGIN: {
		std::cout << "클라이언트 " << c_id << " 로그인 패킷 도착" << std::endl;
		//CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		//strcpy_s(clients[c_id]._name, p->name);
		clients[c_id].send_login_player_packet();

		clients[c_id].send_add_portal_packet(clients[c_id].m_player.portal);

		std::cout << std::endl;
		for (auto& pl : clients) { // 새로 추가된 클라를 모두에게 전송
			if (false == pl.b_use) continue;
			if (pl.m_player.m_id == c_id) continue;
			pl.send_add_player_packet(clients[c_id].m_player);
			pl.send_add_portal_packet(clients[c_id].m_player.portal);
			//std::cout << "클라이언트 " << pl.m_player.m_id << "에게 " << c_id << "의 ADD 패킷을 전송" << std::endl;
			//std::cout << "-> id: " << c_id << " x: " << clients[c_id].m_player.location.x << " y: "<< clients[c_id].m_player.location.y << " z: " << clients[c_id].m_player.location.z << std::endl;
		}
		for (auto& pl : clients) { // 새로 추가된 클라에게 모두 전송
			if (false == pl.b_use) continue;
			if (pl.m_player.m_id == c_id) continue;
			clients[c_id].send_add_player_packet(pl.m_player);
			clients[c_id].send_add_portal_packet(pl.m_player.portal);

			//std::cout << "클라이언트 " << c_id << "에게 " << pl.m_player.m_id << "의 ADD 패킷을 전송" << std::endl;
			//std::cout << "-> id: " << pl.m_player.m_id << " x: " << pl.m_player.location.x << " y: " << pl.m_player.location.y << " z: " << pl.m_player.location.z << std::endl;

		}
		break;
	}
	case CS_MOVE: {

		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		clients[c_id].m_player.SetWorldLocation(p->x, p->y, p->z);
		clients[c_id].m_player.SetWorldRotation(p->pitch, p->yaw, p->roll);
		if (clients[c_id].m_player.location.z < -100.0f) {
			for (auto& pl : clients) {
				if (true == pl.b_use)
					pl.send_respawn_packet(clients[c_id].m_player);
			}
		}
		else {
			//std::cout << "클라이언트 " <<c_id << "x:" << p->x << " y : " << p->y << " z : " << p->z << " 무브 패킷 도착" << std::endl;
			for (auto& pl : clients)
				if (true == pl.b_use && pl.m_player.m_id != c_id)
					pl.send_move_player_packet(clients[c_id].m_player);
		}	
		break;
	}
	case CS_ADD_BLOCK: {
		CS_ADD_BLOCK_PACKET* p = reinterpret_cast<CS_ADD_BLOCK_PACKET*>(packet);

		std::cout << "블록 추가 ->Chunk Index: " << p->chunk_index << ", Indices: (" << p->ix << ", " << p->iy << ", " << p->iz << ")" << std::endl;
		/*	if (map.AddBlockToMap(p)) {
				for (auto& pl : clients)
					if (true == pl.b_use)
						pl.send_add_block_packet(packet);
			}*/
		if (memcmp(&prev_add, p, sizeof(CS_ADD_BLOCK_PACKET)) == 0) {// 클라에서 같은 패킷이 여러번 올 경우 1번만 보내도록 하기 위함->이유를 알 수가 없다..
			break;
		}

		std::memcpy(&prev_add, p, sizeof(CS_ADD_BLOCK_PACKET));
		for (auto& pl : clients) {
			if (true == pl.b_use)
				pl.send_add_block_packet(packet);
		}

		break;
	}
	case CS_REMOVE_BLOCK: {
		CS_REMOVE_BLOCK_PACKET* p = reinterpret_cast<CS_REMOVE_BLOCK_PACKET*>(packet);
		std::cout << "블록 삭제 ->Chunk Index: " << p->chunk_index << ", Indices: (" << p->ix << ", " << p->iy << ", " << p->iz << ")" << std::endl;
		std::cout << "폭파 블럭 타입: " << p->collapsibleType;
	/*		if (map.RemoveBlockToMap(p)) {
				for (auto& pl : clients)
					if (true == pl.b_use)
						pl.send_remove_block_packet(packet);
			}	*/
		if (memcmp(&prev_remove, p, sizeof(CS_REMOVE_BLOCK_PACKET)) == 0) 
			break;

		else {
			std::memcpy(&prev_add, p, sizeof(CS_REMOVE_BLOCK_PACKET));
			for (auto& pl : clients) {
				if (true == pl.b_use)
					pl.send_remove_block_packet(packet);
			}
		}

		break;
	}

	case CS_CHANGE_PLAYER_HP: {
		CS_CHANGE_PLAYER_HP_PACKET* p = reinterpret_cast<CS_CHANGE_PLAYER_HP_PACKET*>(packet);

		if (clients[p->hit_id].m_player.m_hp > 0.f) {
			clients[p->hit_id].m_player.m_hp -= 20.f;
			std::cout << "클라이언트 " << p->hit_id << " 공격당함, 남은 hp: " << clients[p->hit_id].m_player.m_hp << std::endl;
			if (clients[p->hit_id].m_player.m_hp > 0.f) {
				clients[p->hit_id].send_player_hp_packet(clients[p->hit_id].m_player); // 0 보다 크면 자기 자신한테만 전송
			}

			else { // 플레이어만 사망했을 경우
				if (clients[p->hit_id].m_player.portal.m_hp > 0.f) {
					for (auto& pl : clients) {
						if (true == pl.b_use)
							pl.send_respawn_packet(clients[p->hit_id].m_player); // 리스폰만 모두에게 전송
					}
					clients[p->hit_id].m_player.m_hp = 100.f;
				}
				else {
					// 포탈 제거 + 플레이어 사망 시 플레이어 제거 패킷 전송
					for (auto& pl : clients) {
						/*if (true == pl.b_use)
							pl.send_remove_player_packet(clients[p->hit_id].m_player);*/
						if (true == pl.b_use) {
							clients[p->hit_id].m_player.m_hp = -10.f;
							clients[p->hit_id].send_player_hp_packet(clients[p->hit_id].m_player);
						}
					}
				}
			}
		}


		break;
	}

	case CS_CHANGE_PORTAL_HP: {
		CS_CHANGE_PORTAL_HP_PACKET* p = reinterpret_cast<CS_CHANGE_PORTAL_HP_PACKET*>(packet);
		if (clients[p->hit_id].m_player.portal.m_hp > 0.f) {
			clients[p->hit_id].m_player.portal.m_hp -= 300.f;
			std::cout << "클라이언트 " << p->hit_id << " 포탈 공격당함, 남은 hp: " << clients[p->hit_id].m_player.portal.m_hp << std::endl;

			if (clients[p->hit_id].m_player.portal.m_hp > 0.f) {
				clients[p->hit_id].send_portal_hp_packet(clients[p->hit_id].m_player.portal);
			}

			else {
				for (auto& pl : clients) {
					if (true == pl.b_use)
						pl.send_destroy_portal_packet(clients[p->hit_id].m_player.portal);
				}
			}
		}
		break;
	}

	case ANIM: {
		ANIM_PACKET* anim = reinterpret_cast<ANIM_PACKET*>(packet);
		for (auto& pl : clients) {
			if (true == pl.b_use && c_id == anim->id)
				pl.send_anim_packet(anim);
		}
		break;
	}
		

	case CS_DISCONNECT: {
		CS_DISCONNECT_PACKET* p = reinterpret_cast<CS_DISCONNECT_PACKET*>(packet);
		disconnect(p->id);
		break;
	}

	case CS_STRESS_TEST: {
		CS_STRESS_TEST_PACKET* p = reinterpret_cast<CS_STRESS_TEST_PACKET*>(packet);
		clients[c_id].m_player.last_move_time = p->move_time;
		////std::cout << p->move_time << std::endl; // -> 잘 옴
		//clients[c_id].m_player.SetWorldLocation(p->x, p->y, p->z);
		//clients[c_id].m_player.SetWorldRotation(p->pitch, p->yaw, p->roll);	
		//std::cout << "클라이언트 " <<c_id << "x:" << p->x << " y : " << p->y << " z : " << p->z << " 무브 패킷 도착" << std::endl;
		for (auto& pl : clients)
			if (true == pl.b_use)
				pl.send_stress_test_packet(clients[c_id].m_player);
				//pl.send_stress_test_packet2(packet);
		break;
	}

	default:
		std::cout << "잘못된 패킷 수신, PacketType: " << packet[1] << std::endl;
		break;
	}
}

int main()
{
	HANDLE h_iocp;

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(server, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), h_iocp, 9999, 0);
	c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	a_over._op_type = OP_ACCEPT;
	AcceptEx(server, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(worker_thread, h_iocp);

	for (auto& thread : worker_threads)
		thread.join();

	closesocket(server);
	WSACleanup();
}

void worker_thread(HANDLE h_iocp)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		EX_OVERLAPPED* ex_over = reinterpret_cast<EX_OVERLAPPED*>(over);
		if (FALSE == ret) {
			if (ex_over->_op_type == OP_ACCEPT) {
				cout << "Accept Error";
				exit(-1);
			}
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_op_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		switch (ex_over->_op_type) {
		case OP_ACCEPT: {
			int client_id = get_new_client_id();
			if (client_id != -1) {
				clients[client_id].b_use = true;
				clients[client_id].m_player.m_id = client_id;
				clients[client_id].m_player.portal.m_id = client_id;

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

				// 테스트용 시작 좌표
				switch (client_id % 4) {
				case 0:
					clients[client_id].m_player.SetWorldLocation(700.f, 700.f, 1000.f);
					clients[client_id].m_player.SetRespawnLocation(700.f, 700.f, 1000.f);
					clients[client_id].m_player.portal.SetWorldLocation(950.f, 950.f, 700.f);
					break;
				case 1:
					clients[client_id].m_player.SetWorldLocation(-700.f, -700.f, 1000.f);
					clients[client_id].m_player.SetRespawnLocation(-700.f, -700.f, 1000.f);
					clients[client_id].m_player.portal.SetWorldLocation(-950.f, -950.f, 700.f);
					break;
				case 2:
					clients[client_id].m_player.SetWorldLocation(700.f, -700.f, 1000.f);
					clients[client_id].m_player.SetRespawnLocation(700.f, -700.f, 1000.f);
					clients[client_id].m_player.portal.SetWorldLocation(950.f, -950.f, 700.f);
					break;
				case 3:
					clients[client_id].m_player.SetWorldLocation(-700.f, 700.f, 1000.f);
					clients[client_id].m_player.SetRespawnLocation(-700.f, 700.f, 1000.f);
					clients[client_id].m_player.portal.SetWorldLocation(-950.f, 950.f, 700.f);
					break;
				}

				clients[client_id].m_prev_remain = 0;
				clients[client_id].m_socket = c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket),
					h_iocp, client_id, 0);
				clients[client_id].do_recv();
			}
			else {
				cout << "Max user exceeded.\n";
				closesocket(c_socket);
			}
			c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			ZeroMemory(&a_over._over, sizeof(a_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(server, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + clients[key].m_prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
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
}
