#pragma once

//���̹߳�����ö��
enum MutiTaskEnum
{
	MT_None = 0,
	MT_PlayerManger, 
	MT_RoomManger,
	MT_ALL
};

//���̹߳�����ӿ�
class MutiTaskInterface
{
public:
	MutiTaskInterface(MutiTaskEnum index) : m_mt_index(index) {};
	virtual MutiTaskEnum GetMutiTaskIndex() { return m_mt_index; }

	//���߳�Ԥ����׶�
	virtual bool PreMutiTaskStep() = 0;
	//���̴߳���׶�
	virtual void DealMutiTaskStep() = 0;
	//���̺߳���׶�
	virtual void PostMutiTaskStep() = 0;

private:
	MutiTaskEnum m_mt_index;
};