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
	//注册多线程管理类函数
	void RegisterMutiTask(MutiTaskInterface* muti_task);
	//处理单一多线程函数，用于有严格时序要求的多线程处理
	void DealMutiTask(MutiTaskEnum index);
	//处理全部多线程管理类
	void DealAllMutiTask();
	//当前是否是多线程阶段
	bool IsMutiTaskStep();
	//当前是否是某个特定的多线程阶段
	bool IsCurMutiTask(MutiTaskEnum muti_task);

	////////////////////多线程数据安全处理 start//////////////////////////////
	//void SetRecordLock(std::string info);

	//使用锁创建物品
	void CreateItemWithLock(Player* player);

	//使用安全容器处理添加物品
	void PushPlayerItemToSafeMap(Player* player);
	void CreateItemWithSafeMap();

	//使用操作队列处理添加物品
	void PushPlayerItemToQueue(Player* player, int item_num);
	void CreateItemWithQueue();
	////////////////////多线程数据安全处理 end//////////////////////////////

private:
	void DealMutiTask(MutiTaskInterface* task);

private:
	std::map<MutiTaskEnum, MutiTaskInterface*> m_muti_task_map;
	ItemManager* m_item_manager;

	//添加物品队列
	wdmap::map<Player*, int> m_safe_rev_item_map;
	std::deque<SingleOperation> m_deque;

	//当前处理的多线程类型
	MutiTaskEnum m_cur_muti_task;
};

