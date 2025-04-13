#pragma once

class CTimer
{
public:
	CTimer();
	// 생성할 때 프레임 전달
	CTimer(float);

	void Tick(float = 60.0f);
private:
	const float kframe{};
	__int64 current_performance_counter;
	__int64 last_performance_counter;
	double seconds_per_frame;
};

