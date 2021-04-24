#pragma once
#include "ByteStream.h"

//�Ӻ������̵߳Ĳ�������
enum SingleOpType
{
	SOT_None,
	SOT_AddItem,
};

//���������б�������
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


//�����Ʒ����
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