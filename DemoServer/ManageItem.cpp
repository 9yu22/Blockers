#include "ManageItem.h"
#include <iostream>
#include <mutex>
#include <random>

Item::Item()
{
	item_id = -1;
	item_type = NONE;
}

Item::Item(int item_id, Item_type item_type, int location_number) : item_id(item_id), item_type(item_type)
{
	switch (item_type) {
	case STONE: // 아이템은 종류별로 4가지 위치에 스폰된다.
		if (location_number == 0) {
			SetWorldLocation(7000.f, 7000.f, 300.f);
		}
		else if (location_number == 1) {
			SetWorldLocation(-7000.f, 7000.f, 300.f);
		}
		else if (location_number == 2) {
			SetWorldLocation(7000.f, -7000.f, 300.f);
		}
		else if (location_number == 3) {
			SetWorldLocation(-7000.f, -7000.f, 300.f);
		}
		break;

	case AMETHYST:
		if (location_number == 0) {
			SetWorldLocation(7000.f, 7000.f, 300.f);
		}
		else if (location_number == 1) {
			SetWorldLocation(-7000.f, 7000.f, 300.f);
		}
		else if (location_number == 2) {
			SetWorldLocation(7000.f, -7000.f, 300.f);
		}
		else if (location_number == 3) {
			SetWorldLocation(-7000.f, -7000.f, 300.f);
		}
		break;

	case RUBY:
		if (location_number == 0) {
			SetWorldLocation(2000.f, 2000.f, 700.f);
		}
		else if (location_number == 1) {
			SetWorldLocation(-2000.f, 2000.f, 700.f);
		}
		else if (location_number == 2) {
			SetWorldLocation(2000.f, -2000.f, 700.f);
		}
		else if (location_number == 3) {
			SetWorldLocation(-2000.f, -2000.f, 700.f);
		}
		break;

	case DIAMOND:
		if (location_number == 0) {
			SetWorldLocation(3000.f, 3000.f, 1800.f);
		}
		else if (location_number == 1) {
			SetWorldLocation(-3000.f, 3000.f, 1800.f);
		}
		else if (location_number == 2) {
			SetWorldLocation(3000.f, -3000.f, 1800.f);
		}
		else if (location_number == 3) {
			SetWorldLocation(-3000.f, -3000.f, 1800.f);
		}
		break;
	}
}

Item::~Item()
{
}





ItemSpawnManager::ItemSpawnManager()
{
	//manage_items[STONE].item_type = STONE;
	//manage_items[AMETHYST].item_type = AMETHYST;
	//manage_items[RUBY].item_type = RUBY;
	//manage_items[DIAMOND].item_type = DIAMOND;
	
	// 아이템 리젠 시간 설

	// 아이템 시간 계산 초기값 설정

	// 스위치 내부에 아이템 스폰 위치 아이템별로 지정 필요
}

ItemSpawnManager::~ItemSpawnManager()
{
}
void ItemSpawnManager::Init()
{
	item_spawn_time[STONE] = 5000;
	item_spawn_time[AMETHYST] = 10000;
	item_spawn_time[RUBY] = 20000;
	item_spawn_time[DIAMOND] = 30000;

	for (auto& cst : calculate_spawn_time) {
		cst = -5000;
	}
}

void ItemSpawnManager::UpdateTime(time_t duration_time)
{
	for (auto& cst : calculate_spawn_time) {
		cst += duration_time;
	}
}

// 이 함수는 계속 부를건데 이렇게 해도 되는지 모르겠다. 시간 변수를 정수로 하는게 맞는 것 같은데..
// 부르는 간격 자체가 굉장히 짧아서 미미한 데이터 레이스가 생겨도 문제될 것 같지는 않다.
Item_type ItemSpawnManager::CheckCanSpawn()
{
	for (int i = 0; i < 4; i++) // 타입도 0부터 시작하므로..나중에 수정이 필요할 듯
	{
		if (calculate_spawn_time[i] > item_spawn_time[i]) {
			calculate_spawn_time[i] = 0;
			return static_cast<Item_type>(i);
		}
	}
	return NONE;
}
