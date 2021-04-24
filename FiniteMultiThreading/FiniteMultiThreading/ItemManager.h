#pragma once
class ItemManager
{
public:
	ItemManager();
	~ItemManager();
	//创造物品
	int CreateItem();
	//重置id
	void Reset();
	//获取创造了多少个物品
	int GetSize() { return m_pstid; }

private:
	int m_pstid;
};

