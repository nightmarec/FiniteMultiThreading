#pragma once
#include "MutiTaskInterface.h"
#include "ItemManager.h"
#include "ThreadSafeMap.h"
#include "SafeMap.h"
#include "SingleOperation.h"
class Player;
class MutiTaskManager
{
public:
	MutiTaskManager(ItemManager* item_manager);
	~MutiTaskManager();
	//ע����̹߳����ຯ��
	void RegisterMutiTask(MutiTaskInterface* muti_task);
	//����һ���̺߳������������ϸ�ʱ��Ҫ��Ķ��̴߳���
	void DealMutiTask(MutiTaskEnum index);
	//����ȫ�����̹߳�����
	void DealAllMutiTask();
	//��ǰ�Ƿ��Ƕ��߳̽׶�
	bool IsMutiTaskStep();
	//��ǰ�Ƿ���ĳ���ض��Ķ��߳̽׶�
	bool IsCurMutiTask(MutiTaskEnum muti_task);

	////////////////////���߳����ݰ�ȫ���� start//////////////////////////////
	//void SetRecordLock(std::string info);

	//ʹ����������Ʒ
	void CreateItemWithLock(Player* player);

	//ʹ�ð�ȫ�������������Ʒ
	void PushPlayerItemToSafeMap(Player* player);
	void CreateItemWithSafeMap();

	//ʹ�ò������д��������Ʒ
	void PushPlayerItemToQueue(Player* player, int item_num);
	void CreateItemWithQueue();
	////////////////////���߳����ݰ�ȫ���� end//////////////////////////////

private:
	void DealMutiTask(MutiTaskInterface* task);

private:
	std::map<MutiTaskEnum, MutiTaskInterface*> m_muti_task_map;
	ItemManager* m_item_manager;

	//�����Ʒ����
	wdmap::map<Player*, int> m_safe_rev_item_map;
	std::deque<SingleOperation> m_deque;

	//��ǰ����Ķ��߳�����
	MutiTaskEnum m_cur_muti_task;
};

