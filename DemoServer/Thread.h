#pragma once
#include "IOCPServer.h"

class Thread
{
	IOCPServer& server;
public:
	Thread(IOCPServer& server);

	void RunWorkerThread();
	void RunTimerThread();
	time_t UpdateTimer(time_t& last_time);
};