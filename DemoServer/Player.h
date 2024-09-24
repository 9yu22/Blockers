#pragma once
#include <array>
#include "GameObject.h"
//#include "ManageItem.h"
#include "Portal.h"

class Player : public GameObject
{
public:
	int m_id;
	float m_hp;
	WorldRotation m_rotation;
	WorldLocation respawn_location;
	//std::array<Item, 1> m_player_items;
	Portal portal;

	// 스트레스 테스트용
	int last_move_time;

public:
	Player();
	~Player();

	void SetRespawnLocation(float x, float y, float z);
	WorldRotation GetWorldRotation();
	void SetWorldRotation(WorldRotation new_rotation);
	void SetWorldRotation(float new_pitch, float new_yaw, float new_roll);
};

