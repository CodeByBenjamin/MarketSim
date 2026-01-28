#pragma once

#include "TradeStrategy.h"

class TrendStrategy : public TradeStrategy
{
public:
	void decide(Trader& trader, LimitOrderBook& LOB, double time) override;
};