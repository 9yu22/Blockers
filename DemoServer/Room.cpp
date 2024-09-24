#include "Room.h"

Room::Room()
{
}

Room::~Room()
{
}

void Room::StartGame()
{
	last_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	room_state = PLAY;
}

void Room::UpdateTimer()
{
	int last_epoch_time = last_time;
	int now_epoch_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	
	// 타이머 스레드 따로 만들거라..
	//if (atomic_compare_exchange_strong(&last_time, &last_epoch_time, now_epoch_time)) {
	//	in_game_timer++;
	//}
	if (last_epoch_time != now_epoch_time) {
		last_epoch_time = now_epoch_time;
	}
}

void Room::EndGame()
{
}
