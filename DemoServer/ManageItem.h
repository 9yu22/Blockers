#pragma once
#include "GameObject.h"
#include <array>
#include <chrono>
#include <atomic>

enum Item_type { STONE, AMETHYST, RUBY, DIAMOND, NONE };

class Item : public GameObject
{
public:
	int item_id;
	Item_type item_type;

public:
	Item();
	Item(int id, Item_type type, int location_number);

	~Item();
};

class ItemSpawnManager
{
public:
	//std::array<Item, 4> manage_items;
	std::array<time_t, 4> item_spawn_time;
	std::array<time_t, 4> calculate_spawn_time;

	int count_id = 0;

public:
	ItemSpawnManager();
	~ItemSpawnManager();

	//void SpawnItem(Item_type type);
	void Init();
	void UpdateTime(time_t dutarion_time);
	Item_type CheckCanSpawn();
};