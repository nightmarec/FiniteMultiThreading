#pragma once
#include "RoomScene.h"
#include "MutiTaskManager.h"
class RoomManager : public MutiTaskInterface
{
public:
	RoomManager(MutiTaskEnum mt_index, MutiTaskManager* muti_task_manager);
	~RoomManager();

	//���߳�Ԥ����׶�
	virtual bool PreMutiTaskStep();
	//���̴߳���׶�
	virtual void DealMutiTaskStep();
	//���̺߳���׶�
	virtual void PostMutiTaskStep();

	//��������
	RoomScene* CreateRoomScene();

	//���̸߳���
	void UpdateforSingle();

	//���̸߳���
	void UpdateforMuti();

private:
	//��ҵ��̸߳���
	void PlayerUpdateforSingle();
	//�������Ե��̸߳���
	void RoomUpdateForSingle();

	//��Ҷ��̸߳���
	void PlayerUpdateforMuti();
	//�������Զ��̸߳���
	void RoomUpdateForMuti();

private:
	std::vector<RoomScene*> m_room_scene_list;
	MutiTaskManager* m_muti_task_manager;
	int m_cost_time[4];
};

