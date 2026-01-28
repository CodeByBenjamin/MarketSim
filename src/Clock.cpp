#include "Clock.h"

using Time = long long;

Time Clock::now() const
{
	return curTime;
}

void Clock::advance(Time dt)
{
	curTime += dt;
}