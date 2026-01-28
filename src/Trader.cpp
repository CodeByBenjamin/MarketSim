#include "Trader.h"
#include "TrendStrategy.h"

Trader::Trader()
	: strategy(new TrendStrategy),
	funds(1000),
	stocks(100)
{}

Trader::Trader(TradeStrategy* strategy, double funds, long stocks)
	: strategy(strategy),
	funds(funds),
	stocks(stocks)
{}

Trader::~Trader()
{
	delete strategy;
}

double Trader::getFunds() const
{
	return funds;
}

double Trader::getStocks() const
{
	return stocks;
}

void Trader::setFunds(double funds)
{
	this->funds = funds;
}

void Trader::setStocks(long stocks)
{
	this->stocks = stocks;
}

void Trader::update(LimitOrderBook& LOB, double time)
{
	strategy->decide(*this, LOB, time);
}