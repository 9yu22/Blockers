#pragma once
#include <array>
#include <chrono>
#include <atomic>
#include <mutex>
#include "Session.h"
#include "ManageItem.h"

enum ROOM_STATE { EMPTY, PLAY, LOBBY };

class Room
{
public:
	std::array<Session*, 4> clients;
	ItemSpawnManager item_manager;
	std::mutex room_mutex;
	/*std::atomic<int> in_game_timer = 0;
	std::atomic<int> last_time;*/
	int in_gmae_timer = 0;
	int last_time;
	int room_state = EMPTY;

	int set_item_id = 0;
	bool is_timer = false;

public:
	Room();
	~Room();

	void SetPlayerInfo(int num);
	void StartGame();
	void ProcessPacket(int c_id, char* packet);
	void UpdateTimer();
	void EndGame();
	void DisconnectClient(int c_id); // 급한대로 가져옴
};

