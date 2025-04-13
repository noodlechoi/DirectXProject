#include "Timer.h"
#include <Windows.h>

CTimer::CTimer()
{
	__int64 frequency{};
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&current_performance_counter);
	last_performance_counter = current_performance_counter;

	// 1.0 / 시작 이후 지난 시간
	seconds_per_frame = 1.0 / (current_performance_counter / frequency);
}

CTimer::CTimer(float fps)
{
	__int64 frequency{};
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&current_performance_counter);
	last_performance_counter = current_performance_counter;

	// 1.0 / 시작 이후 지난 시간
	seconds_per_frame = 1.0 / fps;

}

void CTimer::Tick(float lock_fps)
{
	
}
