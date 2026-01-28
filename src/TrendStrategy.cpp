#include <cmath>
#include <random>

#include "TrendStrategy.h"
#include "Trader.h"
#include "LimitOrderBook.h"

void TrendStrategy::decide(Trader& trader, LimitOrderBook& LOB, double time)
{
	static std::mt19937 rng(std::random_device{}());
	size_t depth = 5; //How many last trades to look at

	auto const& midPriceHistory = LOB.getMidPriceHistory();

	if (midPriceHistory.empty())
		return;

	double sum = 0;
	size_t count = 0;
	for (auto it = midPriceHistory.rbegin();
		it != midPriceHistory.rend() && count < depth;
		++it, ++count)
	{
		sum += *it;
	}

	size_t actualDepth = std::min(depth, midPriceHistory.size());
	double avr = sum / actualDepth;

	if (avr <= 0.0) return;

	double currentPrice = midPriceHistory.back();
	double diff = currentPrice - avr;

	if (diff > 0)
	{
		auto const& asks = LOB.getAsks();
		if (asks.empty()) return;

		auto bestAskIt = asks.begin();
		if (bestAskIt == asks.end()) return;

		double executionPrice = bestAskIt->first;
		long bestAskVolume = 0;

		for (const auto& orderAtPrice : bestAskIt->second)
		{
			bestAskVolume += orderAtPrice.volume;
		}

		double funds = trader.getFunds();
		long canBuy = std::min(static_cast<long>(std::floor(funds / executionPrice)), bestAskVolume);

		if (canBuy <= 0) return;

		double perc = diff / avr;

		long bias = std::clamp(
			static_cast<long>(std::round(perc * 500)),
			-10L,
			10L
		);

		std::uniform_int_distribution<long> dist(0, canBuy);
		long willBuy = std::clamp(
			dist(rng) + (bias * (canBuy / 20)), 
			1L,
			canBuy
		);

		Order order = { 0, 101, executionPrice, willBuy, Side::BUY, time };
		LOB.processOrder(order);
	}
	else if (diff < 0)
	{
		auto const& bids = LOB.getBids();
		if (bids.empty()) return;

		auto bestBidIt = bids.begin();
		if (bestBidIt == bids.end()) return;

		double executionPrice = bestBidIt->first;
		long bestBidVolume = 0;

		for (const auto& orderAtPrice : bestBidIt->second)
		{
			bestBidVolume += orderAtPrice.volume;
		}

		long stocks = trader.getStocks();
		long canSell = std::min(stocks, bestBidVolume);

		if (canSell <= 0) return;

		double perc = -diff / avr;

		long bias = std::clamp(
			static_cast<long>(std::round(perc * 500)),
			-10L,
			10L
		);

		std::uniform_int_distribution<long> dist(0, canSell);
		long willSell = std::clamp(
			dist(rng) + (bias * (canSell / 20)),
			1L,
			canSell
		);

		Order order = { 0, 101, executionPrice, willSell, Side::SELL, time };
		LOB.processOrder(order);
	}
}