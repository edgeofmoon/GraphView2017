#include "MyTimer.h"


MyTimer::MyTimer(void)
{
	mStartClock = -1;
	mEndClock = -1;
	mPauseStartClock = -1;
	mPauseClockTotal = 0;
}


MyTimer::~MyTimer(void)
{
}

void MyTimer::ResetTiming(){
	mStartClock = std::clock();
	mPauseStartClock = -1;
	mPauseClockTotal = 0;
}

void MyTimer::StartTimerIfNotYet(){
	if(mStartClock < 0){
		this->ResetTiming();
	}
}

void MyTimer::FreshEndTime(){
	mEndClock = std::clock();
	// when end, the paused timer ends too
	if(mPauseStartClock < 0){
		mPauseStartClock = mEndClock;
	}
}

void MyTimer::PauseTimer(){
	// only pause when it is not paused
	if(mPauseStartClock < 0){
		mPauseStartClock = std::clock();
		// also refresh end timer
		mEndClock = mPauseStartClock;
	}
}

void MyTimer::ResumeTimer(){
	// can only resume if paused
	if(mPauseStartClock >= 0){
		mPauseClockTotal += (std::clock()-mPauseStartClock);
		// can be paused again
		mPauseStartClock = -1;
	}
}

double MyTimer::GetUsedTime(){
	return (mEndClock - mStartClock - mPauseClockTotal) / (double) CLOCKS_PER_SEC;
}

double MyTimer::GetCurrentTime(){
	return clock() / (double) CLOCKS_PER_SEC;
}