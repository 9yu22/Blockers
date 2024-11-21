#include <iostream>
#include <array>
//#include <WS2tcpip.h>
//#include <MSWSock.h>
#include "Session.h"
#include "protocol.h"
#include "Map.h"
#include "ManageItem.h"
#include "IOCPServer.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
//using namespace std;
//constexpr int MAX_USER = 5000;

SOCKET c_socket, s_socket;
//EX_OVERLAPPED a_over;

//array<Session, MAX_USER> clients;
Map map;
ItemSpawnManager item_manager;

IOCPServer server;

int set_item_id = 0;
bool is_timer = false;

void worker_thread();

//int get_new_client_id()
//{
//	for (int i = 0; i < MAX_USER; ++i) {
//		lock_guard<mutex> assign_id{ clients[i].session_mx };
//		if (clients[i].b_use == false)
//			return i;
//	}
//		
//	return -1;
//}

//void disconnect(int c_id)
//{
//	for (auto& pl : clients) {
//		{
//			lock_guard<mutex> mx(clients[c_id].session_mx);
//			if (true != pl.b_use) continue;
//		}
//		pl.send_remove_player_packet(clients[c_id].m_player);		
//	}
//	closesocket(clients[c_id].m_socket);
//	std::cout << "Ŭ���̾�Ʈ " << c_id << " ���� ����" << std::endl;
//
//	{
//		lock_guard<mutex> mx(clients[c_id].session_mx);
//		clients[c_id].b_use = false;
//	}
//	
//}

void process_packet(int c_id, char* packet)
{
	static CS_ADD_BLOCK_PACKET prev_add;
	static CS_REMOVE_BLOCK_PACKET prev_remove;

	//if (!is_timer) {
	//	item_manager.InitStartTime();
	//	is_timer = true;
	//}

	//// �ϴ� �ӽ÷� ���⿡..
	//item_manager.CalculateItemSpawnTime(); // �ð� ���
	//Item_type type = item_manager.CheckCanSpawn(); // ���� �ð��� �� �������� ��ȯ

	//if (type != NONE) { // ��� Ŭ�󿡰� �ش� ������(4�� ��ġ) ����
	//	for (int i = 0; i < 4; i++) {
	//		Item item(++set_item_id, type, i);

	//		for (auto& pl : clients) {
	//			if (false == pl.b_use) continue;
	//			pl.send_add_item_packet(item);
	//			//switch (item.item_type) {
	//			//case STONE:
	//			//	std::cout << "Ŭ���̾�Ʈ " <<pl.m_player.m_id << "���� �� ����" << std::endl;
	//			//	break;

	//			//case AMETHYST:
	//			//	std::cout << "Ŭ���̾�Ʈ " << pl.m_player.m_id << "���� �ڼ��� ����" << std::endl;
	//			//	break;

	//			//case RUBY:
	//			//	std::cout << "Ŭ���̾�Ʈ " << pl.m_player.m_id << "���� ��� ����" << std::endl;
	//			//	break;

	//			//case DIAMOND:
	//			//	std::cout << "Ŭ���̾�Ʈ " << pl.m_player.m_id << "���� ���̾� ����" << std::endl;
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
		std::cout << "Ŭ���̾�Ʈ " << c_id << " �α��� ��Ŷ ����" << std::endl;
		//CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		//strcpy_s(clients[c_id]._name, p->name);
		clients[c_id].send_login_player_packet();

		clients[c_id].send_add_portal_packet(clients[c_id].m_player.portal);

		std::cout << std::endl;
		for (auto& pl : clients) { // ���� �߰��� Ŭ�� ��ο��� ����
			if (false == pl.b_use) continue;
			if (pl.m_player.m_id == c_id) continue;
			pl.send_add_player_packet(clients[c_id].m_player);
			pl.send_add_portal_packet(clients[c_id].m_player.portal);
			//std::cout << "Ŭ���̾�Ʈ " << pl.m_player.m_id << "���� " << c_id << "�� ADD ��Ŷ�� ����" << std::endl;
			//std::cout << "-> id: " << c_id << " x: " << clients[c_id].m_player.location.x << " y: "<< clients[c_id].m_player.location.y << " z: " << clients[c_id].m_player.location.z << std::endl;
		}
		for (auto& pl : clients) { // ���� �߰��� Ŭ�󿡰� ��� ����
			if (false == pl.b_use) continue;
			if (pl.m_player.m_id == c_id) continue;
			clients[c_id].send_add_player_packet(pl.m_player);
			clients[c_id].send_add_portal_packet(pl.m_player.portal);

			//std::cout << "Ŭ���̾�Ʈ " << c_id << "���� " << pl.m_player.m_id << "�� ADD ��Ŷ�� ����" << std::endl;
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
			//std::cout << "Ŭ���̾�Ʈ " <<c_id << "x:" << p->x << " y : " << p->y << " z : " << p->z << " ���� ��Ŷ ����" << std::endl;
			for (auto& pl : clients)
				if (true == pl.b_use && pl.m_player.m_id != c_id)
					pl.send_move_player_packet(clients[c_id].m_player);
		}	
		break;
	}
	case CS_ADD_BLOCK: {
		CS_ADD_BLOCK_PACKET* p = reinterpret_cast<CS_ADD_BLOCK_PACKET*>(packet);

		std::cout << "��� �߰� ->Chunk Index: " << p->chunk_index << ", Indices: (" << p->ix << ", " << p->iy << ", " << p->iz << ")" << std::endl;
		/*	if (map.AddBlockToMap(p)) {
				for (auto& pl : clients)
					if (true == pl.b_use)
						pl.send_add_block_packet(packet);
			}*/
		if (memcmp(&prev_add, p, sizeof(CS_ADD_BLOCK_PACKET)) == 0) {// Ŭ�󿡼� ���� ��Ŷ�� ������ �� ��� 1���� �������� �ϱ� ����->������ �� ���� ����..
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
		std::cout << "��� ���� ->Chunk Index: " << p->chunk_index << ", Indices: (" << p->ix << ", " << p->iy << ", " << p->iz << ")" << std::endl;
		std::cout << "���� �� Ÿ��: " << p->collapsibleType;
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
			std::cout << "Ŭ���̾�Ʈ " << p->hit_id << " ���ݴ���, ���� hp: " << clients[p->hit_id].m_player.m_hp << std::endl;
			if (clients[p->hit_id].m_player.m_hp > 0.f) {
				clients[p->hit_id].send_player_hp_packet(clients[p->hit_id].m_player); // 0 ���� ũ�� �ڱ� �ڽ����׸� ����
			}

			else { // �÷��̾ ������� ���
				if (clients[p->hit_id].m_player.portal.m_hp > 0.f) {
					for (auto& pl : clients) {
						if (true == pl.b_use)
							pl.send_respawn_packet(clients[p->hit_id].m_player); // �������� ��ο��� ����
					}
					clients[p->hit_id].m_player.m_hp = 100.f;
				}
				else {
					// ��Ż ���� + �÷��̾� ��� �� �÷��̾� ���� ��Ŷ ����
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
			std::cout << "Ŭ���̾�Ʈ " << p->hit_id << " ��Ż ���ݴ���, ���� hp: " << clients[p->hit_id].m_player.portal.m_hp << std::endl;

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
		////std::cout << p->move_time << std::endl; // -> �� ��
		//clients[c_id].m_player.SetWorldLocation(p->x, p->y, p->z);
		//clients[c_id].m_player.SetWorldRotation(p->pitch, p->yaw, p->roll);	
		//std::cout << "Ŭ���̾�Ʈ " <<c_id << "x:" << p->x << " y : " << p->y << " z : " << p->z << " ���� ��Ŷ ����" << std::endl;
		for (auto& pl : clients)
			if (true == pl.b_use)
				pl.send_stress_test_packet(clients[c_id].m_player);
				//pl.send_stress_test_packet2(packet);
		break;
	}

	default:
		std::cout << "�߸��� ��Ŷ ����, PacketType: " << packet[1] << std::endl;
		break;
	}
}

int main()
{
	server.StartServer();

	std::vector <std::thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(worker_thread);

	for (auto& thread : worker_threads)
		thread.join();

	closesocket(s_socket);
	WSACleanup();
}

void worker_thread()
{
	server.Run();
}
