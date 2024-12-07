#include <iostream>
#include <array>
//#include <WS2tcpip.h>
//#include <MSWSock.h>
#include "Session.h"
#include "protocol.h"
#include "Map.h"
#include "ManageItem.h"
#include "IOCPServer.h"
#include "Thread.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

IOCPServer server;

int main()
{
	server.StartServer();

	Thread thread(server);

	std::vector <std::thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency() - 1; // 워커 스레드 하나 줄이고 하나는 타이머 스레드 생성
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(&Thread::RunWorkerThread, &thread);
	 
	std::thread timer_thread(&Thread::RunTimerThread, &thread);
	timer_thread.join();

	for (auto& thread : worker_threads)
		thread.join();

	server.EndServer();
}
