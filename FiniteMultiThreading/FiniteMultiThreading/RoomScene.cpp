#include "stdafx.h"
#include "RoomScene.h"
#include "Timer.h"
#include <assert.h>

RoomScene::RoomScene(MutiTaskManager* muti_task_manager)
	: m_muti_task_manager(muti_task_manager)
	, m_muti_task_id(0)
{
}


RoomScene::~RoomScene()
{
}

void RoomScene::JoinRoom(Player* player)
{
	m_player_list.push_back(player);
	player->SetRoomScene(this);
}

void RoomScene::Update()
{
	sleep_windows(10);
	std::vector<Player*>::iterator iter = m_player_list.begin();
	for (; iter != m_player_list.end(); ++iter)
	{
		if (*iter == NULL)
			continue;

		(*iter)->UpdateForRoom();
	}
}

void RoomScene::CheckMutiTask()
{
	if (!m_muti_task_manager->IsCurMutiTask(MT_RoomManger))
		return;
	
	assert((int)omp_get_thread_num() == m_muti_task_id);
}