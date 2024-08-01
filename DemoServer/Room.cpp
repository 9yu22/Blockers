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
	item_manager.Init();
	room_state = PLAY;
}

void Room::UpdateTimer()
{
	int last_epoch_time = last_time;
	int now_epoch_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	
	// Ÿ�̸� ������ ���� ����Ŷ�..
	//if (atomic_compare_exchange_strong(&last_time, &last_epoch_time, now_epoch_time)) {
	//	in_game_timer++;
	//}
	if (last_epoch_time != now_epoch_time) {
		last_epoch_time = now_epoch_time;
		item_manager.UpdateTime();
	}
}

void Room::EndGame()
{
}
