#pragma once
#include "Player.h"
#include <vector>
#include "MutiTaskManager.h"

class PlayerManager : public MutiTaskInterface
{
public:
	PlayerManager(MutiTaskEnum mt_index, MutiTaskManager* muti_task_manager);
	~PlayerManager();

	void CreatePlayer(int num);
	void UpdateForMuti();
	void UpdateForSingle();
	Player* GetPlayer(int index);
	void SetQueueFlag(bool flag);

	//多线程预处理阶段
	virtual bool PreMutiTaskStep();
	//多线程处理阶段
	virtual void DealMutiTaskStep();
	//多线程后处理阶段
	virtual void PostMutiTaskStep();

private:
	std::vector<Player*> m_player_list;
	MutiTaskManager* m_muti_task_manager;
	bool m_use_queue;
};

