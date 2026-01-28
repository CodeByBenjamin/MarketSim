#include "Trader.h"
#include "TrendStrategy.h"

Trader::Trader(TradeStrategy* strategy, long id, double funds, long stocks)
	: strategy(strategy),
	id(id),
	funds(funds),
	stocks(stocks)
{}

long Trader::getId() const
{
	return id;
}

double Trader::getFunds() const
{
	return funds;
}

double Trader::getStocks() const
{
	return stocks;
}

const std::vector<long>& Trader::getActiveOrderIds() const
{
	return activeOrders;
}

void Trader::changeFunds(double funds)
{
	this->funds += funds;
}

void Trader::changeStocks(long stocks)
{
	this->stocks += stocks;
}

void Trader::update(LimitOrderBook& LOB, Clock& clock)
{
	strategy->decide(*this, LOB, clock);
}

void Trader::addActiveOrderId(long id)
{
	activeOrders.push_back(id);
}

void Trader::clearActiveOrderIds()
{
	activeOrders.clear();
}