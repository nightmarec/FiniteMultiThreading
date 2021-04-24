#pragma once
#include "RoomScene.h"
class RoomScene;
class Player
{
public:
	Player(int player_index, int num);
	~Player();

	int GetPlayerIndex() { return m_player_index; }
	void Update();
	void UpdateForRoom();
	void SaveItem(int item);
	void SetRoomScene(RoomScene* room_scene) { m_room_scene = room_scene; }
	void DoSomethingInRoom();


private:
	int m_player_index;
	int m_random;
	std::set<int> m_package;
	RoomScene* m_room_scene;
};

