#include <iostream>
#include "Thread.h"

Thread::Thread(IOCPServer& server) : server(server)
{

}

void Thread::RunWorkerThread()
{
	server.Run();
}

void Thread::RunTimerThread()
{
	time_t last_time;

	while (true) {
		time_t duration_time = UpdateTimer(last_time);

		for (auto& room : server.rooms) {
			if (room.room_state == PLAY) {
				room.ProcessTimerEvent(duration_time);
			}
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		//std::cout << "Test Timer Thread 1 Dutarion Time: " << duration_time << "ms" << std::endl;
	}
}

time_t Thread::UpdateTimer(time_t& last_time)
{
	std::time_t now_epoch_time = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();

	return now_epoch_time - last_time;
}
