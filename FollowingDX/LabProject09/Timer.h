#pragma once

class CTimer {
public:
	CTimer();

	void Tick(float = 60.0f);
	size_t GetFrameRate(LPTSTR, int);
	float GetTimeElapsed() { return time_elapsed; }
	void Reset();
private:
	const static size_t kMaxSampleCount{ 50 };
	
	__int64 performance_frequency;

	__int64 current_performance_counter{};
	__int64 last_performance_counter;

	double time_sacle;

	float time_elapsed;

	float frame_time[kMaxSampleCount];
	size_t sample_cnt{};
	double frame_per_seconds{};
	float fps_time_elapsed{};
	size_t current_frame_rate{};

	bool used_performance_counter{};

	bool is_stop{};
};

