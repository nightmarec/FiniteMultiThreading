#include "stdafx.h"
#include "RocordLock.h"
#include "Timer.h"
#include <iostream>


RocordLock::RocordLock(std::string info, omp_lock_t * _Lock)
{
	m_lock = _Lock; //赋值锁
	static std::string old_info; //锁信息
	//尝试进行锁，不冲突直接锁且返回true，冲突返回false
	if (!omp_test_lock(m_lock))
	{
		//冲突后，使用微秒级计时器计时
		RecordTimer timer;
		timer.start();
		omp_set_lock(m_lock); //等待锁施放
		timer.stop();
		//记录两个冲突的日志以及冲突时间
		std::cout << "old lock info: " << old_info.c_str() << " ,new lock info: " << info.c_str() << ", Lock time:" << timer.elapsed() << std::endl;
	}

	old_info = info;//更新锁信息
}

RocordLock::~RocordLock()
{
	omp_unset_lock(m_lock);
}