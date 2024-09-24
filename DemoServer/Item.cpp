#include "Item.h"
#include <iostream>
#include <mutex>
#include <random>

Item::Item()
{
	item_id = -1;
	item_type = NONE;
}

Item::~Item()
{
}

void Item::SetItemInfo(Item_type item_type, int location_number)
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
