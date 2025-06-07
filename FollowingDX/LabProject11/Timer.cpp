#include "stdafx.h"
#include "Timer.h"

CTimer::CTimer()
{
	// ���� ���ļ��� ����� �� ������ ��Ƽ �̵�� Ÿ�̸� ���
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&performance_frequency)) {
		used_performance_counter = true;
		QueryPerformanceCounter((LARGE_INTEGER*)&last_performance_counter);
		time_sacle = 1.0 / (double)performance_frequency;
	}
	else {
		used_performance_counter = false;
		last_performance_counter = timeGetTime();
		time_sacle = 0.001f;	// miilisecond
	}

}

void CTimer::Tick(float lockFps)
{
	if (is_stop)
	{
		time_elapsed = 0.0f;
		return;
	}
	
	if (used_performance_counter)
		::QueryPerformanceCounter((LARGE_INTEGER*)&current_performance_counter);
	else
		current_performance_counter = timeGetTime();

	float timeElapse = float((current_performance_counter - last_performance_counter) * time_sacle);

	if (lockFps > 0.0f)
	{
		// ���ϴ� delta time�� �ƴϸ� loop
		while (timeElapse < (1.0f / lockFps))
		{
			if (used_performance_counter)
				::QueryPerformanceCounter((LARGE_INTEGER*)&current_performance_counter);
			else
				current_performance_counter = timeGetTime();

			timeElapse = float((current_performance_counter - last_performance_counter) * time_sacle);
		}
	}

	last_performance_counter = current_performance_counter;

	// ������ ������ ó�� �ð��� ���� ������ ó�� �ð��� ���̰� 1�ʺ��� ������ ���� ������ ó�� �ð��� frame_time[0]�� ����
	if (fabsf(timeElapse - time_elapsed) < 1.0f)
	{
		::memmove(&frame_time[1], frame_time, (kMaxSampleCount - 1) * sizeof(float));
		frame_time[0] = timeElapse;
		if (sample_cnt < kMaxSampleCount) ++sample_cnt;
	}

	// ������++, ���� ������ ó�� �ð��� ���� �ϰ� ����
	++frame_per_seconds;
	fps_time_elapsed += timeElapse;
	if (fps_time_elapsed > 1.0f) {
		current_frame_rate = frame_per_seconds;
		frame_per_seconds = 0;
		fps_time_elapsed = 0.0f;
	}

	// ���ø�
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

void CTimer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&last_performance_counter);

	current_performance_counter = last_performance_counter;

	is_stop = false;
}
