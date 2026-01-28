#pragma once

#include <map>
#include <unordered_map>
#include <list>
#include <vector>

#include "datatypes.h"

namespace sf {
    class RenderWindow;
    class Font;
}

class LimitOrderBook
{
private:
	std::map<double, std::list<Order>, std::greater<double>> bids;
	std::map<double, std::list<Order>> asks;

	std::unordered_map<long, std::list<Order>::iterator> orderLookup;

	long nextOrderId = 1;

	double lastTradePrice = 0.0;
	std::vector<TradeRecord> tradeRecords;
	std::vector<double> midPriceRecords;

	long nextTradeId = 1;
public:

	const std::map<double, std::list<Order>, std::greater<double>>& getBids() const;
	const std::map<double, std::list<Order>>& getAsks() const;
	long getHighestVolume(Side side, size_t priceLevels) const;

	void update();

	const std::vector<TradeRecord>& getTradeHistory() const;
	const std::vector<double>& getMidPriceHistory() const;

	void processOrder(const Order& incomingOrder);
	void executeMatch(Order& incomingOrder);
	void addLimitOrder(Order incomingOrder);
	bool cancelOrder(long orderId);

	void recordTrade(const Order& restingOrder, const Order& incomingOrder, long volume, double price);

	const std::vector<DepthPoint> depthChartPoints(float binSize, long* totalVolume) const;
};