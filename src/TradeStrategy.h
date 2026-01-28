#pragma once

class Trader;
class LimitOrderBook;

class TradeStrategy
{
public:
	virtual void decide(Trader& trader, LimitOrderBook& LOB, double time) = 0;
};