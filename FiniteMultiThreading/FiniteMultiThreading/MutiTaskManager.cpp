#include "stdafx.h"
#include "MutiTaskManager.h"
#include "Player.h"
#include "PlayerManager.h"
#include <iostream>
#include <stdlib.h>
#include "Timer.h"
#include <string>
#include "RocordLock.h"
using namespace std;
static omp_lock_t item_lock;

MutiTaskManager::MutiTaskManager(ItemManager* item_manager)
	: m_item_manager(item_manager)
	, m_cur_muti_task(MT_None)
{
	omp_init_lock(&item_lock); // 初始化互斥锁
}


MutiTaskManager::~MutiTaskManager()
{
	omp_unset_lock(&item_lock); //释放互斥器
}


void MutiTaskManager::RegisterMutiTask(MutiTaskInterface* muti_task)
{
	m_muti_task_map[muti_task->GetMutiTaskIndex()] = muti_task;
}

void MutiTaskManager::DealMutiTask(MutiTaskEnum index)
{
	std::map<MutiTaskEnum, MutiTaskInterface*>::iterator iter = m_muti_task_map.find(index);
	if (iter == m_muti_task_map.end())
		return;

	DealMutiTask(iter->second);
}

void MutiTaskManager::DealAllMutiTask()
{
	std::map<MutiTaskEnum, MutiTaskInterface*>::iterator iter = m_muti_task_map.begin();
	for (; iter != m_muti_task_map.end(); ++iter)
	{
		DealMutiTask(iter->second);
	}
}

void MutiTaskManager::DealMutiTask(MutiTaskInterface* task)
{
	if (task == NULL)
		return;

	task->PreMutiTaskStep();

	//多线程处理阶段
	m_cur_muti_task = task->GetMutiTaskIndex();
	task->DealMutiTaskStep();
	m_cur_muti_task = MT_None;

	task->PostMutiTaskStep();

	//删除队列中的操作
	m_deque.clear();
	//m_safe_rev_item_map.clear();
}

bool MutiTaskManager::IsMutiTaskStep()
{
	return m_cur_muti_task != MT_None;
}

bool MutiTaskManager::IsCurMutiTask(MutiTaskEnum muti_task)
{
	return m_cur_muti_task == muti_task;
}

void MutiTaskManager::CreateItemWithLock(Player* player)
{
	if (!player || !m_item_manager)
		return;

	int item = 0;
	if (IsMutiTaskStep())
	{
		std::string info = "pid=" + std::to_string(player->GetPlayerIndex());
		RocordLock r_lock(info, &item_lock);
		item = m_item_manager->CreateItem();
	}
	else
	{
		item = m_item_manager->CreateItem();
	}	

	player->SaveItem(item);
}

void MutiTaskManager::PushPlayerItemToQueue(Player* player, int item_num)
{
	if (!player || !m_item_manager)
		return;
	SingleAdditemOperation item;
	item.m_player_id = player->GetPlayerIndex();
	item.m_item_num = 1;
	
	SingleOperation sop(SOT_AddItem);
	COStream ostr(sop.m_data);
	item.SaveStream(ostr);

	std::string info = "pid=" + std::to_string(player->GetPlayerIndex());
	RocordLock r_lock(info, &item_lock); 
	m_deque.push_back(sop);
}

void MutiTaskManager::CreateItemWithQueue()
{
	if (!m_item_manager)
		return;

	std::map<MutiTaskEnum, MutiTaskInterface*>::iterator iter_mgr = m_muti_task_map.find(MT_PlayerManger);
	if (iter_mgr == m_muti_task_map.end())
		return;
	PlayerManager* player_manager = static_cast<PlayerManager*>(iter_mgr->second);

	std::deque<SingleOperation>::iterator iter = m_deque.begin();
	for (; iter != m_deque.end(); ++iter)
	{
		if (iter->type == SOT_AddItem)
		{
			CIStream istr(iter->m_data);

			SingleAdditemOperation add_item_op;
			add_item_op.LoadStream(istr);

			Player* player = player_manager->GetPlayer(add_item_op.m_player_id);
			if (!player)
				continue;

			int item = m_item_manager->CreateItem();
			player->SaveItem(item);
		}
	}
}

void MutiTaskManager::PushPlayerItemToSafeMap(Player* player)
{
	if (!player || !m_item_manager)
		return;

	m_safe_rev_item_map[player] ++;
}

void MutiTaskManager::CreateItemWithSafeMap()
{
	wdmap::map<Player*, int>::iterator iter = m_safe_rev_item_map.begin();
	for (; iter != m_safe_rev_item_map.end(); ++iter)
	{
		int item = m_item_manager->CreateItem();
		iter.first->SaveItem(item);
	}
}

/*void MutiTaskManager::SetRecordLock(std::string info)
{
	static std::string old_info;
	if (!omp_test_lock(&item_lock))
	{
		//计时
		RecordTimer timer;
		timer.start();
		omp_set_lock(&item_lock);
		timer.stop();

		std::cout << "old lock info: " << old_info.c_str() << " ,cur lock info: " << info.c_str() << ", Lock time:"  << timer.elapsed() << std::endl;
	}

	old_info = info;
}*/