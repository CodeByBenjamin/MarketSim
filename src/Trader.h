#pragma once

#include "TradeStrategy.h"

enum TraderType
{
	Sentiment,
	Contrarian,
	LongTerm,
	Whale
};

class Trader {
private:
	TradeStrategy* strategy;

	double funds;
	long stocks;
public:
	Trader();
	Trader(TradeStrategy* strategy, double funds, long stocks);
	~Trader();

	double getFunds() const;
	double getStocks() const;

	void setFunds(double funds);
	void setStocks(long stocks);
	
	void update(LimitOrderBook& LOB, double time);
};