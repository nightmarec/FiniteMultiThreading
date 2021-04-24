#pragma once
#include "ByteStream.h"

//延后至单线程的操作类型
enum SingleOpType
{
	SOT_None,
	SOT_AddItem,
};

//操作队列中保存数据
class SingleOperation
{
public:
	SingleOperation(SingleOpType sot)
		: type(sot)
	{}

public:
	SingleOpType type;
	CDynamicStreamBuf m_data;
};


//添加物品操作
class SingleAdditemOperation
{
public:
	SingleAdditemOperation() 
		: m_player_id(0)
		, m_item_num(0)
	{

	}
public:
	int m_player_id;
	int m_item_num;

public:
	virtual void SaveStream(COStream& to)
	{
		to << m_player_id << m_item_num;
	}

	virtual void LoadStream(CIStream& from)
	{
		from >> m_player_id >> m_item_num;
	}
};