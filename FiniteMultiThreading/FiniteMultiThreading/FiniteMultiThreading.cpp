// FiniteMultiThreading.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <omp.h>  // openMP必带头文件
#include <stdio.h>
#include "PlayerManager.h"
#include "MutiTaskManager.h"
#include "RoomManager.h"
#include "Timer.h"
using namespace std;

// void test() {
// 	for (int i = 0; i < 50000; i++) {
// 		int x = 1000, y = 100;
// 		double  num = x / y;
// 	}
// }

// void OMPTest()
// {
// 	const int count = 50000;
// 	int a[count];
// 	int b[count];
// 	int c[count] = { 0 };
// 	for (int i = 0; i < count; i++) {
// 		a[i] = i + 1;
// 		b[i] = 10;
// 	}
// 
// 	cout << "CPU number:" << omp_get_num_procs() << endl;
// 
// #pragma omp parallel 
// 	{
// 		printf("Hello!   threadID=%d  thraed number:%d\n", omp_get_thread_num(), omp_get_num_threads());
// 	}
// 
// 	//use openMP instruct to parallel to loop;
// 	//set the  threads num 
// 	omp_set_num_threads(4);
// 
// 	double start = omp_get_wtime();
// 	//Also can use:#pragma omp parallel for  num_threads(4)
// #pragma omp parallel for  
// 	for (int i = 0; i < count; i++) {
// 		c[i] = a[i] / b[i];
// 		test();
// 	}
// 	double end = omp_get_wtime();
// 	cout << "Multi-thread Time is: " << end - start << endl;
// 
// 
// 	double t1 = omp_get_wtime();
// 	for (int i = 0; i < count; i++) {
// 		c[i] = a[i] / b[i];
// 		test();
// 	}
// 	double t2 = omp_get_wtime();
// 	cout << "Single Time is: " << t2 - t1 << endl;
// }

// void UpdateForSingle()
// {
// 	sleep_windows(4000);
// }
// 
// void UpdateForMuti()
// {
// #pragma omp parallel sections
// 	{
// 		#pragma omp section
// 		{
// 			sleep_windows(1000);
// 		}
// 		#pragma omp section
// 		{
// 			sleep_windows(1000);
// 		}
// 		#pragma omp section
// 		{
// 			sleep_windows(1000);
// 		}
// 		#pragma omp section
// 		{
// 			sleep_windows(1000);
// 		}
// 	}
// }

int main() {

	int player_num = 100;
	int room_player_num = 10;
	int room_num = player_num / room_player_num;

	//构造物品类
	ItemManager item_manager;
	//有限多线程类
	MutiTaskManager muti_task_manager(&item_manager);

	//构造玩家管理类
	PlayerManager player_manager(MT_PlayerManger, &muti_task_manager);
	player_manager.CreatePlayer(player_num);

	//构造房间管理类
	RoomManager room_manager(MT_RoomManger, &muti_task_manager);
	for (int i = 0; i < room_num; ++i)
	{
		RoomScene* room = room_manager.CreateRoomScene();
		if (!room)
			continue;

		for (int j = 0; j < room_player_num; ++j)
		{
			room->JoinRoom(player_manager.GetPlayer(i*room_player_num + j));
		}	
	}

	//注册多任务管理类
	muti_task_manager.RegisterMutiTask(&player_manager);
	muti_task_manager.RegisterMutiTask(&room_manager);

	//游戏循环
	while (true)
	{
		//模拟单线程
		item_manager.Reset();
		double start_single = omp_get_wtime();
		player_manager.UpdateForSingle();
		room_manager.UpdateforSingle();
		double end_single = omp_get_wtime();
		cout << "Single Time is: " << end_single - start_single << endl;
		cout << "item_manager item_size: " << item_manager.GetSize() << endl;

		//模拟多线程-整个添加物品使用锁
		item_manager.Reset();
		player_manager.SetQueueFlag(false);
		double start_lock = omp_get_wtime();
		muti_task_manager.DealAllMutiTask();
		double end_lock = omp_get_wtime();
		cout << "Lock Muti Time is: " << end_lock - start_lock << endl;
		cout << "item_manager item_size: " << item_manager.GetSize() << endl;

		//模拟多线程-添加物品使用操作队列
		item_manager.Reset();
		player_manager.SetQueueFlag(true);
		double start_queue = omp_get_wtime();
		muti_task_manager.DealAllMutiTask();
		double end_queue = omp_get_wtime();
		cout << "Queue Muti Time is: " << end_queue - start_queue << endl;
		cout << "item_manager item_size: " << item_manager.GetSize() << endl;

		system("Pause");
	}

	system("Pause");
	return 0;
}

