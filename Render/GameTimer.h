//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

//#ifndef GAMETIMER_H
//#define GAMETIMER_H

#pragma once

#include "D3DUtility.h"

namespace Render {

class GameTimer
{
public:
	RENDERDLL_API GameTimer();

	RENDERDLL_API float TotalTime()const;  // in seconds
	RENDERDLL_API float DeltaTime()const; // in seconds

	RENDERDLL_API void Reset(); // Call before message loop.
	RENDERDLL_API void Start(); // Call when unpaused.
	RENDERDLL_API void Stop();  // Call when paused.
	RENDERDLL_API void Tick();  // Call every frame.

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

//#endif // GAMETIMER_H


}

