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
	case STONE: // �������� �������� 4���� ��ġ�� �����ȴ�.
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
	
	// ������ ���� �ð� ����
	item_spawn_time[STONE] = 5.0;
	item_spawn_time[AMETHYST] = 10.0;
	item_spawn_time[RUBY] = 20.0;
	item_spawn_time[DIAMOND] = 30.0;

	// ������ �ð� ��� �ʱⰪ ����
	for (auto& cst: calculate_spawn_time){
		cst = -5.0;
	}

	// ����ġ ���ο� ������ ���� ��ġ �����ۺ��� ���� �ʿ�
}

ItemSpawnManager::~ItemSpawnManager()
{
}

//void ManageItem::SpawnItem(Item_type type)
//{
//	switch (type) {
//	case STONE:
//		manage_items[STONE].is_spawned = true;
//		//manage_items[STONE].calculate_spawn_time = 0.0;
//		spawn_item_type = STONE;
//		break;
//
//	case AMETHYST:
//		manage_items[AMETHYST].is_spawned = true;
//		manage_items[AMETHYST].calculate_spawn_time = 0.0;
//		spawn_item_type = AMETHYST;
//		break;
//
//	case RUBY:
//		//manage_items[RUBY].is_spawned = true;
//		manage_items[RUBY].calculate_spawn_time = 0.0;
//		spawn_item_type = RUBY;
//		break;
//
//	case DIAMOND:
//		manage_items[DIAMOND].is_spawned = true;
//		//manage_items[DIAMOND].calculate_spawn_time = 0.0;
//		spawn_item_type = DIAMOND;
//		break;
//
//	default:
//		spawn_item_type = NONE;
//		break;
//	}
//
//}

void ItemSpawnManager::UpdateTimeForSpawn(double elapsedTime)
{
	for (auto& cst : calculate_spawn_time) {
		cst += elapsedTime;
	}
}

void ItemSpawnManager::InitStartTime()
{
	lastUpdateTime = std::chrono::steady_clock::now();
}


// �� �Լ��� ��� �θ��ǵ� �̷��� �ص� �Ǵ��� �𸣰ڴ�. �ð� ������ ������ �ϴ°� �´� �� ������..
// �θ��� ���� ��ü�� ������ ª�Ƽ� �̹��� ������ ���̽��� ���ܵ� ������ �� ������ �ʴ�.
Item_type ItemSpawnManager::CheckCanSpawn()
{
	for (int i = 0; i < 4; i++) // Ÿ�Ե� 0���� �����ϹǷ�..���߿� ������ �ʿ��� ��
	{
		if (calculate_spawn_time[i] > item_spawn_time[i]) {
			calculate_spawn_time[i] = 0.0;
			return static_cast<Item_type>(i);
		}
	}
	return NONE;
}

void ItemSpawnManager::CalculateItemSpawnTime()
{
	int compareTimeForInt = calculateTimeForInt;
	int changeTime = compareTimeForInt + 1;
	auto currentTime = std::chrono::steady_clock::now();

	// ���� �ð��� ���� �ð��� ���� ���
	//std::chrono::duration<double> elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUpdateTime);
	std::chrono::duration<double> elapsedTime = currentTime - lastUpdateTime;

	if (elapsedTime.count() >= 1.0) {
		// CAS�� �ϴ� ������ process_packet���� ��Ŷ�� ���� �� ���� �ð��� ������Ʈ �ϱ� ����
		//if (std::atomic_compare_exchange_strong(&calculateTimeForInt, &compareTimeForInt, changeTime)) {
			//std::cout << "CAS ����" << std::endl;
			lastUpdateTime = currentTime;
			UpdateTimeForSpawn(elapsedTime.count());
		//}
	}
	
}
