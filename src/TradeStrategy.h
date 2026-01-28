#pragma once

class Trader;
class LimitOrderBook;
class Clock;

class TradeStrategy
{
public:
	virtual void decide(Trader& trader, LimitOrderBook& LOB, Clock& clock) = 0;
};