#include <iostream>
#include "Session.h"

Session::Session()
{
	b_use = false;
	m_prev_remain = 0;
	room_num = -1;
}

Session::~Session()
{
}

void Session::do_recv()
{
	DWORD recv_flag = 0;
	memset(&_recv_over._over, 0, sizeof(_recv_over._over));
	_recv_over._wsabuf.len = BUF_SIZE - m_prev_remain;
	_recv_over._wsabuf.buf = _recv_over._send_buf + m_prev_remain;
	WSARecv(m_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
		&_recv_over._over, 0);
}

void Session::do_send(void* packet)
{
	EX_OVERLAPPED* sdata = new EX_OVERLAPPED{ reinterpret_cast<char*>(packet) };
	//char* data = reinterpret_cast<char*>(packet);
	//if (data[1] > 20) {
	//	std::cout << "잘못된 패킷 전송, PacketType: " << data[1] << std::endl;
	//}

	WSASend(m_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
}

void Session::send_login_player_packet()
{
	SC_LOGIN_INFO_PACKET login_player;
	login_player.id = m_player.m_id;
	login_player.size = sizeof(SC_LOGIN_INFO_PACKET);
	login_player.type = SC_LOGIN_INFO;
	login_player.x = m_player.location.x;
	login_player.y = m_player.location.y;
	login_player.z = m_player.location.z;
	//std::cout << "id: " << p.id << " x:" << p.x << " y: " << p.y << " z: " << p.z << " 로그인 패킷 전송" << std::endl;
	do_send(&login_player);
}

void Session::send_move_player_packet(Player player)
{
	SC_MOVE_PLAYER_PACKET move_player;
	move_player.id = player.m_id;
	move_player.size = sizeof(SC_MOVE_PLAYER_PACKET);
	move_player.type = SC_MOVE_PLAYER;
	move_player.x = player.location.x;
	move_player.y = player.location.y;
	move_player.z = player.location.z;
	move_player.pitch = player.m_rotation.pitch;
	move_player.yaw = player.m_rotation.yaw;
	move_player.roll = player.m_rotation.roll;
	//std::cout << "클라이언트 " << c_id << "x:" << p.x << " y : " << p.y << " z : " << p.z << " 무브 패킷 전송" << std::endl << std::endl;
	do_send(&move_player);
}

void Session::send_add_player_packet(Player player)
{
	SC_ADD_PLAYER_PACKET add_player;
	add_player.id = player.m_id;
	//strcpy_s(add_packet.name, p->name);
	add_player.size = sizeof(add_player);
	add_player.type = SC_ADD_PLAYER;
	add_player.x = player.location.x;
	add_player.y = player.location.y;
	add_player.z = player.location.z;
	do_send(&add_player);
}

void Session::send_add_block_packet(char* packet)
{
	SC_ADD_BLOCK_PACKET* add_block = reinterpret_cast<SC_ADD_BLOCK_PACKET*>(packet);
	add_block->size = sizeof(SC_ADD_BLOCK_PACKET);
	add_block->type = SC_ADD_BLOCK;
	do_send(add_block);
}

void Session::send_remove_block_packet(char* packet)
{
	SC_REMOVE_BLOCK_PACKET* remove_block = reinterpret_cast<SC_REMOVE_BLOCK_PACKET*>(packet);
	remove_block->size = sizeof(SC_REMOVE_BLOCK_PACKET);
	remove_block->type = SC_REMOVE_BLOCK;
	do_send(remove_block);
}

void Session::send_player_hp_packet(Player player)
{
	SC_CHANGE_PLAYER_HP_PACKET change_hp;
	change_hp.size = sizeof(SC_CHANGE_PLAYER_HP_PACKET);
	change_hp.type = SC_CHANGE_PLAYER_HP;
	change_hp.hp = player.m_hp;
	change_hp.id = player.m_id;
	std::cout << "플레이어 " << player.m_id << "의 hp: " << player.m_hp << " 전송" << std::endl;
	do_send(&change_hp);
}

void Session::send_add_portal_packet(Portal portal)
{
	SC_ADD_PORTAL_PACKET add_portal;
	add_portal.size = sizeof(SC_ADD_PORTAL_PACKET);
	add_portal.type = SC_ADD_PORTAL;
	add_portal.hp = portal.m_hp;
	add_portal.id = portal.m_id;
	add_portal.x = portal.location.x;
	add_portal.y = portal.location.y;
	add_portal.z = portal.location.z;
	do_send(&add_portal);
}

void Session::send_portal_hp_packet(Portal portal)
{
	SC_CHANGE_PORTAL_HP_PACKET change_hp;
	change_hp.size = sizeof(SC_CHANGE_PORTAL_HP_PACKET);
	change_hp.type = SC_CHANGE_PORTAL_HP;
	change_hp.hp = portal.m_hp;
	change_hp.id = portal.m_id;
	std::cout << "플레이어 " << portal.m_id << "의 Portal hp: " << portal.m_hp << " 전송" << std::endl;
	do_send(&change_hp);
}

void Session::send_add_item_packet(Item item)
{
	SC_ADD_ITEM_PACKET add_item;
	add_item.size = sizeof(SC_ADD_ITEM_PACKET);
	add_item.type = SC_ADD_ITEM;
	add_item.id = item.item_id;
	add_item.item_type = item.item_type;
	add_item.x = item.location.x;
	add_item.y = item.location.y;
	add_item.z = item.location.z;
	do_send(&add_item);

	//std::cout << std::endl;
}

void Session::send_remove_item_packet(char* packet)
{
	SC_REMOVE_ITEM_PACKET* remove_item = reinterpret_cast<SC_REMOVE_ITEM_PACKET*>(packet);
	remove_item->size = sizeof(SC_REMOVE_ITEM_PACKET);
	remove_item->type = SC_REMOVE_ITEM;
	do_send(&remove_item);
}

void Session::send_respawn_packet(Player player)
{
	SC_RESPAWN_PACKET respawn;
	respawn.size = sizeof(SC_RESPAWN_PACKET);
	respawn.type = SC_RESPAWN;
	respawn.id = player.m_id;
	respawn.hp = 100.f;
	respawn.x = player.respawn_location.x;
	respawn.y = player.respawn_location.y;
	respawn.z = player.respawn_location.z;
	do_send(&respawn);
}

void Session::send_destroy_portal_packet(Portal portal)
{
	SC_DESTROY_PORTAL_PACKET destroy_portal;
	destroy_portal.size = sizeof(SC_DESTROY_PORTAL_PACKET);
	destroy_portal.type = SC_DESTROY_PORTAL;
	destroy_portal.id = portal.m_id;
	do_send(&destroy_portal);
}

void Session::send_remove_player_packet(Player player)
{
	SC_REMOVE_PLAYER_PACKET remove_player;
	remove_player.size = sizeof(SC_REMOVE_PLAYER_PACKET);
	remove_player.type = SC_REMOVE_PLAYER;
	remove_player.id = player.m_id;
	do_send(&remove_player);
}

void Session::send_anim_packet(ANIM_PACKET* anim)
{
	do_send(anim);
}

void Session::send_stress_test_packet(Player player)
{
	SC_STRESS_TEST_PACKET test;
	test.id = player.m_id;
	test.size = sizeof(SC_STRESS_TEST_PACKET);
	test.type = SC_STRESS_TEST;
	test.x = player.location.x;
	test.y = player.location.y;
	test.z = player.location.z;
	test.pitch = player.m_rotation.pitch;
	test.yaw = player.m_rotation.yaw;
	test.roll = player.m_rotation.roll;
	test.move_time = player.last_move_time;
	do_send(&test);
}

void Session::send_stress_test_packet2(char* packet)
{
	SC_STRESS_TEST_PACKET test;
	test.size = sizeof(SC_STRESS_TEST_PACKET);
	test.type = SC_STRESS_TEST;
	do_send(&test);
}


