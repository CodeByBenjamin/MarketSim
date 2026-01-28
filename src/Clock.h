#pragma once

class Clock
{
private:
	using Time = double;

	Time curTime = 0.0;
public:
	Time now() const;

	void advance(Time dt);
};