#pragma once

//多线程管理类枚举
enum MutiTaskEnum
{
	MT_None = 0,
	MT_PlayerManger, 
	MT_RoomManger,
	MT_ALL
};

//多线程管理类接口
class MutiTaskInterface
{
public:
	MutiTaskInterface(MutiTaskEnum index) : m_mt_index(index) {};
	virtual MutiTaskEnum GetMutiTaskIndex() { return m_mt_index; }

	//多线程预处理阶段
	virtual bool PreMutiTaskStep() = 0;
	//多线程处理阶段
	virtual void DealMutiTaskStep() = 0;
	//多线程后处理阶段
	virtual void PostMutiTaskStep() = 0;

private:
	MutiTaskEnum m_mt_index;
};