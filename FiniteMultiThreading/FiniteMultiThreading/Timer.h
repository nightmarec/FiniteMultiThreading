#pragma once
#if defined(__linux__) || defined(__unix__)
#define sleep sleep_unix
#include "sys/socket.h"
#elif defined(_WIN32)
#define sleep sleep_windows
#include <winsock2.h>  
#include <WS2tcpip.h>
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"ws2_32.lib")  
#endif // 
#include "stdarg.h"
#include "time.h"

void sleep_unix(unsigned const int millisecond);
void sleep_windows(unsigned const int millisecond);

//∫¡√Îº∂º∆ ±∆˜∑‚◊∞
class RecordTimer
{
public:
	RecordTimer()
		: elapsed_(0)
	{
		QueryPerformanceFrequency(&freq_);
	}
	~RecordTimer() {}
public:
	void start()
	{
		QueryPerformanceCounter(&begin_time_);
	}
	void stop()
	{
		LARGE_INTEGER end_time;
		QueryPerformanceCounter(&end_time);
		elapsed_ += (end_time.QuadPart - begin_time_.QuadPart) * 1000000 / freq_.QuadPart;
	}
	void restart()
	{
		elapsed_ = 0;
		start();
	}
	//Œ¢√Î
	double elapsed()
	{
		return static_cast<double>(elapsed_);
	}
	//∫¡√Î
	double elapsed_ms()
	{
		return elapsed_ / 1000.0;
	}
	//√Î
	double elapsed_second()
	{
		return elapsed_ / 1000000.0;
	}

private:
	LARGE_INTEGER freq_;
	LARGE_INTEGER begin_time_;
	long long elapsed_;
};
