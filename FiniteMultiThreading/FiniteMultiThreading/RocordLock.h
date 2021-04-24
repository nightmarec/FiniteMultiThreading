#pragma once
#include <omp.h>
class RocordLock
{
public:
	RocordLock(std::string info, omp_lock_t * _Lock);
	~RocordLock();

private:
	omp_lock_t* m_lock;
};

