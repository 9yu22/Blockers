#pragma once
#include "Common.h"
#include "Session.h"
#include "ClientManager.h"

//enum OP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };

class IOCPServer
{
public:
	HANDLE h_iocp;
	SOCKET c_socket, s_socket;
	EX_OVERLAPPED ac_over;
	ClientManager c_manager;

public:
	IOCPServer();
	~IOCPServer();

	void StartServer();
	void ProcessGQCS(OP_TYPE _op_type);
	void Run();

public: // getters
	HANDLE GetIOCPHandle() const { return h_iocp; }
};

