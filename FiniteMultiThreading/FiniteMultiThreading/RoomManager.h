#pragma once
#include "RoomScene.h"
#include "MutiTaskManager.h"
class RoomManager : public MutiTaskInterface
{
public:
	RoomManager(MutiTaskEnum mt_index, MutiTaskManager* muti_task_manager);
	~RoomManager();

	//多线程预处理阶段
	virtual bool PreMutiTaskStep();
	//多线程处理阶段
	virtual void DealMutiTaskStep();
	//多线程后处理阶段
	virtual void PostMutiTaskStep();

	//创建房间
	RoomScene* CreateRoomScene();

	//单线程更新
	void UpdateforSingle();

	//多线程更新
	void UpdateforMuti();

private:
	//玩家单线程更新
	void PlayerUpdateforSingle();
	//房间属性单线程更新
	void RoomUpdateForSingle();

	//玩家多线程更新
	void PlayerUpdateforMuti();
	//房间属性多线程更新
	void RoomUpdateForMuti();

private:
	std::vector<RoomScene*> m_room_scene_list;
	MutiTaskManager* m_muti_task_manager;
	int m_cost_time[4];
};

