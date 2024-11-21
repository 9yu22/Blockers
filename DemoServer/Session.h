#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mutex>
#include "Player.h"
#include "protocol.h"
#include "ManageItem.h"

struct EX_OVERLAPPED {
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	OP_TYPE _op_type;
	char _packet_type;
	EX_OVERLAPPED()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_op_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	EX_OVERLAPPED(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_op_type = OP_SEND;
		_packet_type = packet[1];
		memcpy(_send_buf, packet, packet[0]);
	}
};

class Session // = Client
{
	EX_OVERLAPPED _recv_over;

public:
	SOCKET m_socket;
	Player m_player;

	int m_prev_remain;
	bool b_use;

	std::mutex session_mx;

public:
	Session();
	~Session();

	void do_recv();
	void do_send(void* packet);

	void send_login_player_packet();
	void send_move_player_packet(Player player);
	void send_add_player_packet(Player player);
	void send_add_block_packet(char* packet);
	void send_remove_block_packet(char* packet);
	void send_player_hp_packet(Player player);
	void send_add_portal_packet(Portal portal);
	void send_portal_hp_packet(Portal portal);
	void send_add_item_packet(Item item);
	void send_remove_item_packet(char* packet);
	void send_respawn_packet(Player player);
	void send_destroy_portal_packet(Portal portal);
	void send_remove_player_packet(Player player);
	void send_anim_packet(ANIM_PACKET* anim);

	void send_stress_test_packet(Player player);
	void send_stress_test_packet2(char* packet);
};

