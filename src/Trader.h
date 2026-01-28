#pragma once

#include <vector>

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

	long id;
	double funds;
	long stocks;

	std::vector<long> activeOrders;
public:
	Trader(TradeStrategy* strategy, long id, double funds, long stocks);

	long getId() const;
	double getFunds() const;
	double getStocks() const;
	const std::vector<long>& getActiveOrderIds() const;

	void changeFunds(double funds);
	void changeStocks(long stocks);
	
	void update(LimitOrderBook& LOB, Clock& clock);

	void addActiveOrderId(long id);
	void clearActiveOrderIds();
};