#pragma once
#include "stdafx.h"

class CTimer {
public:
	CTimer();

	void Tick(float = 60.0f);
	size_t GetFrameRate(LPTSTR, int);
	float GetTimeElapsed() { return time_elapsed; }

private:
	const static size_t kMaxSampleCount{ 50 };
	
	__int64 performance_frequency;

	__int64 current_performance_counter{};
	__int64 last_performance_counter;

	double time_sacle;

	float time_elapsed;

	std::array<float, kMaxSampleCount> frame_time;
	size_t sample_cnt{};
	double frame_per_seconds{};
	float fps_time_elapsed;
	size_t current_frame_rate{};

	bool is_stop;
};

