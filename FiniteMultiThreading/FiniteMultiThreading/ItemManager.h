#pragma once
class ItemManager
{
public:
	ItemManager();
	~ItemManager();
	//������Ʒ
	int CreateItem();
	//����id
	void Reset();
	//��ȡ�����˶��ٸ���Ʒ
	int GetSize() { return m_pstid; }

private:
	int m_pstid;
};

