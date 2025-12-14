#include "datatypes.h"

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

	LOBState getLOBState() const;
	const std::vector<TradeRecord>& getTradeHistory() const;
	double getLastTradePrice() const;
	size_t getBookDepth(Side side) const;

	void processOrder(Order incomingOrder);
	bool cancelOrder(long orderId);
	void executeMatch(Order incomingOrder);
	void addLimitOrder(Order incomingOrder);

	void recordTrade(const Order& restingOrder, const Order& incomingOrder, long volume, double price);
};