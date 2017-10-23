#pragma once

#include <ctime>

class MyTimer
{
public:
	MyTimer(void);
	~MyTimer(void);

	void ResetTiming();
	void StartTimerIfNotYet();
	void FreshEndTime();
	void PauseTimer();
	void ResumeTimer();
	double GetUsedTime();

	double GetCurrentTime();

protected:
	
	clock_t mStartClock;
	clock_t mEndClock;

	clock_t mPauseStartClock;
	clock_t mPauseClockTotal;
};

