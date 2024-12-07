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
	
	// ������ ���� �ð� ��

	// ������ �ð� ��� �ʱⰪ ����

	// ����ġ ���ο� ������ ���� ��ġ �����ۺ��� ���� �ʿ�
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

// �� �Լ��� ��� �θ��ǵ� �̷��� �ص� �Ǵ��� �𸣰ڴ�. �ð� ������ ������ �ϴ°� �´� �� ������..
// �θ��� ���� ��ü�� ������ ª�Ƽ� �̹��� ������ ���̽��� ���ܵ� ������ �� ������ �ʴ�.
Item_type ItemSpawnManager::CheckCanSpawn()
{
	for (int i = 0; i < 4; i++) // Ÿ�Ե� 0���� �����ϹǷ�..���߿� ������ �ʿ��� ��
	{
		if (calculate_spawn_time[i] > item_spawn_time[i]) {
			calculate_spawn_time[i] = 0;
			return static_cast<Item_type>(i);
		}
	}
	return NONE;
}
