#include "ItemSpawner.h"

ItemSpawner::ItemSpawner()
{
	for (int i = 0; i < 4; ++i){
		stone[i].SetItemInfo(STONE, i);
		amethyst[i].SetItemInfo(AMETHYST, i);
		ruby[i].SetItemInfo(RUBY, i);
		diamond[i].SetItemInfo(DIAMOND, i);

		calculate_spawn_time[i] = 0;
		is_spawn[i] = false;
	}

	item_spawn_time[STONE] = 5;
	item_spawn_time[AMETHYST] = 10;
	item_spawn_time[RUBY] = 20;
	item_spawn_time[DIAMOND] = 30;
}

ItemSpawner::~ItemSpawner()
{
}

void ItemSpawner::UpdateCalculateSpawnTime()
{
	for (auto& cst : calculate_spawn_time){
		cst++;
	}
}

bool ItemSpawner::CheckIsSpawn()
{
	bool res = false;

	for (int i = 0; i < 4; ++i){
		if (item_spawn_time[i] == calculate_spawn_time[i]) {
			is_spawn[i] = true;
			calculate_spawn_time[i] = 0;
			res = true;
		}
	} 

	return res;
}
