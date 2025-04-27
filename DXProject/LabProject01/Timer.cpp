#include "Timer.h"

CTimer::CTimer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&performance_frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&last_performance_counter);
	time_sacle = 1.0 / (double)performance_frequency;
}

void CTimer::Tick(float lockFps)
{
	if (is_stop)
	{
		time_elapsed = 0.0f;
		return;
	}

	::QueryPerformanceCounter((LARGE_INTEGER*)&current_performance_counter);
	float timeElapse = float((current_performance_counter - last_performance_counter) * time_sacle);

	if (lockFps > 0.0f)
	{
		// 원하는 delta time이 아니면 loop
		while (timeElapse < (1.0f / lockFps))
		{
			::QueryPerformanceCounter((LARGE_INTEGER*)&current_performance_counter);
			timeElapse = float((current_performance_counter - last_performance_counter) * time_sacle);
		}
	}

	last_performance_counter = current_performance_counter;

	// 합리적인 범위 이내일 때만 새 데이터 샘플링
	if (fabsf(timeElapse - time_elapsed) < 1.0f)
	{
		::memmove(&frame_time[1], frame_time.data(), (kMaxSampleCount - 1) * sizeof(float));
		frame_time[0] = timeElapse;
		if (sample_cnt < kMaxSampleCount) ++sample_cnt;
	}

	++frame_per_seconds;
	fps_time_elapsed += timeElapse;
	// 1초가 넘어가면
	if (fps_time_elapsed > 1.0f)
	{
		current_frame_rate = frame_per_seconds;
		frame_per_seconds = 0;
		fps_time_elapsed = 0.0f;
	}

	// 샘플링
	timeElapse = 0.0f;
	for (ULONG i = 0; i < sample_cnt; i++) time_elapsed += frame_time[i];
	if (sample_cnt > 0) time_elapsed /= sample_cnt;
}

size_t CTimer::GetFrameRate(LPTSTR title, int frame)
{
	if (title) {
		_itow_s(current_frame_rate, title, frame, 10);
		wcscat_s(title, frame, _T(" FPS)"));
	}

	return current_frame_rate;
}
