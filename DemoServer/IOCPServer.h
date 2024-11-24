#pragma once
//#include "Common.h"
#include "Session.h"
#include "Room.h"

//enum OP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };

constexpr int MAX_ROOM = 1000;
constexpr int MAX_PLAYER = 4;
constexpr int MAX_USER = MAX_ROOM * MAX_PLAYER;

class IOCPServer
{
public:
	std::array<Session, MAX_USER> clients;
	std::array<Room, MAX_ROOM>rooms;

	HANDLE h_iocp;
	SOCKET c_socket, s_socket;
	EX_OVERLAPPED ac_over;

	DWORD num_bytes;
	ULONG_PTR key;
	EX_OVERLAPPED* ex_over;

public:
	IOCPServer();
	~IOCPServer();

	void StartServer();
	void Run();
	void EndServer();

	void ProcessGQCS(OP_TYPE _op_type);
	//void ProcessPacket(int c_id, char* packet);

	//int LoginClient(SOCKET c_socket);
	void DisconnectClient(int c_id);
	int GetClientId();
	bool AddClientInRoom(Session* s);

public: // getters
	HANDLE GetIOCPHandle() const { return h_iocp; }
};

