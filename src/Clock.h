#pragma once

class Clock
{
private:
	using Time = long long;

	Time curTime = 0LL;
public:
	Time now() const;

	void advance(Time dt);
};