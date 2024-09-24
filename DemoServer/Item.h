#pragma once
#include "GameObject.h"
#include <array>
#include <chrono>
#include <atomic>

enum Item_type { STONE, AMETHYST, RUBY, DIAMOND, NONE };

class Item : public GameObject
{
public:
	int item_id = -1; // 나중에 아이디 할당 및 스폰된 모든 아이템 관리 로직 추가
	Item_type item_type;

public:
	Item();
	~Item();

	void SetItemInfo(Item_type item_type, int location_number);
};
