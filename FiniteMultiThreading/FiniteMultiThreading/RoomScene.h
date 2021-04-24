#pragma once
#include "Player.h"
#include "MutiTaskManager.h"

class RoomScene
{
public:
	RoomScene(MutiTaskManager* muti_task_manager);
	~RoomScene();

	void Update();
	void JoinRoom(Player* player);
	//���õ�ǰ���߳�id
	void SetMutiTaskID(int id) {m_muti_task_id = id;}
	//�����߳��Ƿ�Ϸ�
	void CheckMutiTask();

private:
	std::vector<Player*> m_player_list;
	MutiTaskManager* m_muti_task_manager;
	int m_muti_task_id;
};

