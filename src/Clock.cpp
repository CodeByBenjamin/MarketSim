#include "Clock.h"

using Time = double;

Time Clock::now() const
{
	return curTime;
}

void Clock::advance(Time dt)
{
	curTime += dt;
}