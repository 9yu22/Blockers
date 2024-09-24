#pragma once
#include <array>
#include "Item.h"
class ItemSpawner
{
public:
	std::array<Item, 4> stone;
	std::array<Item, 4> amethyst;
	std::array<Item, 4> ruby;
	std::array<Item, 4> diamond;

	std::array<int, 4> item_spawn_time;
	std::array<int, 4> calculate_spawn_time;
	std::array<bool, 4> is_spawn;

public:
	ItemSpawner();
	~ItemSpawner();

	void UpdateCalculateSpawnTime();
	bool CheckIsSpawn(); // 동시에 여러 종류를 스폰해야 할 수도 있으니까 배열로 반환할까..?
};

