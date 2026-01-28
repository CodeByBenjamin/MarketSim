#pragma once

#include "TradeStrategy.h"

class RandomStrategy : public TradeStrategy
{
public:
	void decide(Trader& trader, LimitOrderBook& LOB, Clock& clock) override;
};