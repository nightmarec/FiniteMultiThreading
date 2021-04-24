#include "stdafx.h"
#include "ItemManager.h"
#include "Timer.h"


ItemManager::ItemManager()
	: m_pstid(0)
{
}


ItemManager::~ItemManager()
{
}

int ItemManager::CreateItem()
{
	sleep_windows(1);
	return m_pstid++;
}

void ItemManager::Reset()
{
	m_pstid = 0;
}

