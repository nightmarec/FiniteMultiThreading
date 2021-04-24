#include "stdafx.h"
#include "Player.h"
#include "Timer.h"

Player::Player(int player_index, int num) 
	: m_player_index(player_index)
	, m_random(num)
	, m_room_scene(NULL)
{
}


Player::~Player()
{
}


void Player::Update()
{
	sleep_windows(m_random);
}

void Player::UpdateForRoom()
{
	DoSomethingInRoom();
}

void Player::SaveItem(int item)
{
	m_package.insert(item);
}

void Player::DoSomethingInRoom()
{
	if (!m_room_scene)
		return;
	m_room_scene->CheckMutiTask();

	// do something
	sleep_windows(m_random);
}


