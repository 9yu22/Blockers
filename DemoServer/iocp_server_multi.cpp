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

//SOCKET c_socket, s_socket;
//EX_OVERLAPPED a_over;

//array<Session, MAX_USER> clients;
//Map map;
//ItemSpawnManager item_manager;
IOCPServer server;

void worker_thread();

int main()
{
	server.StartServer();

	std::vector <std::thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(worker_thread);

	for (auto& thread : worker_threads)
		thread.join();

	server.EndServer();
}

void worker_thread()
{
	server.Run();
}
