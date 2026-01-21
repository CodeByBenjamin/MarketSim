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

	long nextTradeId = 1;
public:

	const std::map<double, std::list<Order>, std::greater<double>>& getBids() const;
	const std::map<double, std::list<Order>>& getAsks() const;
	LOBState getLOBState() const;
	const std::vector<TradeRecord>& getTradeHistory() const;
	double getLastTradePrice() const;
	size_t getBookDepth(Side side) const;

	void processOrder(Order incomingOrder);
	bool cancelOrder(long orderId);
	void executeMatch(Order incomingOrder);
	void addLimitOrder(Order incomingOrder);

	void recordTrade(const Order& restingOrder, const Order& incomingOrder, long volume, double price);

	void draw(sf::RenderWindow& window, const sf::Font& font, float lobWidth);
	const std::vector<DepthPoint> depthChartPoints(float binSize, long* totalVolume) const;
};