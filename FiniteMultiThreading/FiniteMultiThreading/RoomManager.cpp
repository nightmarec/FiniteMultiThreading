#include "stdafx.h"
#include "RoomManager.h"
#include "Timer.h"


RoomManager::RoomManager(MutiTaskEnum mt_index, MutiTaskManager* muti_task_manager)
	: MutiTaskInterface(mt_index)
	, m_muti_task_manager(muti_task_manager)
{
	m_cost_time[0] = 100 + rand() % 100;
	m_cost_time[1] = 100 + rand() % 100;
	m_cost_time[2] = 100 + rand() % 100;
	m_cost_time[3] = 100 + rand() % 100;
}


RoomManager::~RoomManager()
{
	std::vector<RoomScene*>::iterator iter = m_room_scene_list.begin();
	for (; iter != m_room_scene_list.end(); ++iter)
	{
		delete *iter;
		*iter = NULL;
	}

	m_room_scene_list.clear();
}

RoomScene* RoomManager::CreateRoomScene()
{
	RoomScene* room = new RoomScene(m_muti_task_manager);
	m_room_scene_list.push_back(room);
	return room;
}

void RoomManager::RoomUpdateForSingle()
{
	int sum = m_cost_time[0] + m_cost_time[1] + m_cost_time[2] + m_cost_time[3];
	sleep_windows(sum);
}

void RoomManager::RoomUpdateForMuti()
{
#pragma omp parallel sections
	{
#pragma omp section
		{
			sleep_windows(m_cost_time[0]);
		}
#pragma omp section
		{
			sleep_windows(m_cost_time[1]);
		}
#pragma omp section
		{
			sleep_windows(m_cost_time[2]);
		}
#pragma omp section
		{
			sleep_windows(m_cost_time[3]);
		}
	}
}

void RoomManager::PlayerUpdateforSingle()
{
	int sum = m_room_scene_list.size();
	for (int i = 0; i < sum; ++i)
	{
		if (m_room_scene_list[i] == NULL)
			continue;

		m_room_scene_list[i]->Update();
	}
}

void RoomManager::PlayerUpdateforMuti()
{
	int sum = m_room_scene_list.size();

#pragma omp parallel for num_threads(4) schedule(dynamic)
	for (int i = 0; i < sum; ++i)
	{
		if (m_room_scene_list[i] == NULL)
			continue;

		m_room_scene_list[i]->SetMutiTaskID((int)omp_get_thread_num());
		m_room_scene_list[i]->Update();
	}
}

void RoomManager::UpdateforSingle()
{
	RoomUpdateForSingle();
	PlayerUpdateforSingle();
}

void RoomManager::UpdateforMuti()
{
	RoomUpdateForMuti();
	PlayerUpdateforMuti();
}

//多线程预处理阶段
bool RoomManager::PreMutiTaskStep()
{
	return true;
}

//多线程处理阶段
void RoomManager::DealMutiTaskStep()
{
	UpdateforMuti();
}

//多线程后处理阶段
void RoomManager::PostMutiTaskStep()
{

}