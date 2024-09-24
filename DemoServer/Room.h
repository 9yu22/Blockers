#pragma once
#include <array>
#include <chrono>
#include <atomic>
#include <mutex>
#include "Session.h"
#include "Item.h"
#include "ItemSpawner.h"

enum ROOM_STATE { EMPTY, PLAY, LOBBY };

class Room
{
public:
	std::array<Session, 4> sessions;
	ItemSpawner spawner;
	std::mutex room_mutex;
	int elapsedTime = 0;
	int last_time;
	int room_state = EMPTY;


public:
	Room();
	~Room();

	void StartGame();
	void UpdateTimer();
	void EndGame();
};

