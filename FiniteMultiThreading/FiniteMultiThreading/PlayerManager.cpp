#include "stdafx.h"
#include "PlayerManager.h"
#include <assert.h>


PlayerManager::PlayerManager(MutiTaskEnum mt_index, MutiTaskManager* muti_task_manager)
	: MutiTaskInterface(mt_index)
	, m_muti_task_manager(muti_task_manager)
	, m_use_queue(false)
{
}


PlayerManager::~PlayerManager()
{
	std::vector<Player*>::iterator iter = m_player_list.begin();
	for (; iter != m_player_list.end(); ++iter)
	{
		delete *iter;
		(*iter) = NULL;
	}

	m_player_list.clear();
}

void PlayerManager::CreatePlayer(int num)
{
	if (num < 0)
		return;

	for (int i = 0; i < num; ++i)
	{
		int random = 1 + rand() % 5;
		Player* player = new Player(i, random);
		m_player_list.push_back(player);
	}
}

Player* PlayerManager::GetPlayer(int index)
{
	//��ϣ�������player update��ʱ����ô˺�����
	assert(!m_muti_task_manager->IsCurMutiTask(MT_PlayerManger));

	int size = m_player_list.size();
	if (index >= size)
		return NULL;

	return m_player_list[index];
}

void PlayerManager::UpdateForMuti()
{
	int sum = m_player_list.size();

#pragma omp parallel for num_threads(4) schedule(dynamic)
	for (int i = 0; i < sum; ++i)
	{
		if (m_player_list[i] == NULL)
			continue;

 		m_player_list[i]->Update();
		
		//��������ע�ͣ���������̶߳���
		//Player* lover = GetPlayer(i + 1);

		if (m_use_queue)
		{
			//ʹ�ö���Ϊ�����ӵ���
			m_muti_task_manager->PushPlayerItemToQueue(m_player_list[i], 1);
		}
		else
		{
			//ʹ����Ϊ�����ӵ���
			m_muti_task_manager->CreateItemWithLock(m_player_list[i]);
		}
	}
}

void PlayerManager::UpdateForSingle()
{
	int sum = m_player_list.size();
	for (int i = 0; i < sum; ++i)
	{
		if (m_player_list[i] == NULL)
			return;

		m_player_list[i]->Update();
		m_muti_task_manager->CreateItemWithLock(m_player_list[i]);
	}
}

void PlayerManager::SetQueueFlag(bool flag)
{
	m_use_queue = flag;
}

//���߳�Ԥ����׶�
bool PlayerManager::PreMutiTaskStep()
{
	return true;
}

//���̴߳���׶�
void PlayerManager::DealMutiTaskStep()
{
	UpdateForMuti();
}

//���̺߳���׶�
void PlayerManager::PostMutiTaskStep()
{
	m_muti_task_manager->CreateItemWithQueue();
}