#include "stdafx.h"
#include "RocordLock.h"
#include "Timer.h"
#include <iostream>


RocordLock::RocordLock(std::string info, omp_lock_t * _Lock)
{
	m_lock = _Lock; //��ֵ��
	static std::string old_info; //����Ϣ
	//���Խ�����������ͻֱ�����ҷ���true����ͻ����false
	if (!omp_test_lock(m_lock))
	{
		//��ͻ��ʹ��΢�뼶��ʱ����ʱ
		RecordTimer timer;
		timer.start();
		omp_set_lock(m_lock); //�ȴ���ʩ��
		timer.stop();
		//��¼������ͻ����־�Լ���ͻʱ��
		std::cout << "old lock info: " << old_info.c_str() << " ,new lock info: " << info.c_str() << ", Lock time:" << timer.elapsed() << std::endl;
	}

	old_info = info;//��������Ϣ
}

RocordLock::~RocordLock()
{
	omp_unset_lock(m_lock);
}