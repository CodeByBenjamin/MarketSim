#include "lob.h"

LOBState LimitOrderBook::getLOBState() const
{
	LOBState state{};

	if (!bids.empty())
	{
		const auto& bestBidLevel = *bids.begin();

		state.bestBidPrice = bestBidLevel.first;

		long bestBidVolume = 0;

		for (const Order& order : bestBidLevel.second)
		{
			bestBidVolume += order.volume;
		}

		state.bestBidVolume = bestBidVolume;
	}

	if (!asks.empty())
	{
		const auto& bestAskLevel = *asks.begin();

		state.bestAskPrice = bestAskLevel.first;

		long bestAskVolume = 0;

		for (const Order& order : bestAskLevel.second)
		{
			bestAskVolume += order.volume;
		}

		state.bestAskVolume = bestAskVolume;
	}

	return state;
}

const std::vector<TradeRecord>& LimitOrderBook::getTradeHistory() const
{
	return tradeRecords;
}

double LimitOrderBook::getLastTradePrice() const
{
	return lastTradePrice;
}

size_t LimitOrderBook::getBookDepth(Side side) const
{
	if (side == BUY)
		return bids.size();
	else
		return asks.size();
}

void LimitOrderBook::processOrder(Order incomingOrder)
{
	incomingOrder.id = nextOrderId++;

	if (incomingOrder.side == Side::BUY) {
		if (!asks.empty() && incomingOrder.price >= asks.begin()->first)
		{
			executeMatch(incomingOrder);
		}
		else
		{
			addLimitOrder(incomingOrder);
		}
	}
	else
	{
		if (!bids.empty() && incomingOrder.price <= bids.begin()->first)
		{
			executeMatch(incomingOrder);
		}
		else
		{
			addLimitOrder(incomingOrder);
		}
	}
}

void LimitOrderBook::executeMatch(Order incomingOrder) 
{
	if (incomingOrder.side == Side::BUY) 
	{
		auto priceLevelIt = asks.begin();

		while (incomingOrder.volume > 0 && priceLevelIt != asks.end() &&
			priceLevelIt->first <= incomingOrder.price)
		{
			std::list<Order>& priceList = priceLevelIt->second;

			while (incomingOrder.volume > 0 && !priceList.empty())
			{
				Order& restingOrder = priceList.front();
				long tradeVolume = std::min(incomingOrder.volume, restingOrder.volume);

				recordTrade(incomingOrder, restingOrder, tradeVolume, priceLevelIt->first);
				lastTradePrice = priceLevelIt->first;

				restingOrder.volume -= tradeVolume;
				incomingOrder.volume -= tradeVolume;

				if (restingOrder.volume == 0) {
					orderLookup.erase(restingOrder.id);
					priceList.pop_front();
				}
			}

			if (priceList.empty()) {
				priceLevelIt = asks.erase(priceLevelIt);
			}
			else {
				break;
			}
		}

		if (incomingOrder.volume > 0)
		{
			addLimitOrder(incomingOrder);
		}
	}
	else
	{
		auto priceLevelIt = bids.begin();

		while (incomingOrder.volume > 0 && priceLevelIt != bids.end() &&
			priceLevelIt->first >= incomingOrder.price)
		{
			std::list<Order>& priceList = priceLevelIt->second;

			while (incomingOrder.volume > 0 && !priceList.empty())
			{
				Order& restingOrder = priceList.front();
				long tradeVolume = std::min(incomingOrder.volume, restingOrder.volume);

				recordTrade(restingOrder, incomingOrder, tradeVolume, priceLevelIt->first);
				lastTradePrice = priceLevelIt->first;

				restingOrder.volume -= tradeVolume;
				incomingOrder.volume -= tradeVolume;

				if (restingOrder.volume == 0) {
					orderLookup.erase(restingOrder.id);
					priceList.pop_front();
				}
			}

			if (priceList.empty()) {
				priceLevelIt = bids.erase(priceLevelIt);
			}
			else {
				break;
			}
		}

		if (incomingOrder.volume > 0)
		{
			addLimitOrder(incomingOrder);
		}
	}
}

bool LimitOrderBook::cancelOrder(long orderId)
{
	auto mapIt = orderLookup.find(orderId);

	if (mapIt == orderLookup.end()) {
		return false;
	}

	std::list<Order>::iterator listIt = mapIt->second;
	const Order& orderToCancel = *listIt;

	if (orderToCancel.side == Side::BUY) {
		auto priceLevelIt = bids.find(orderToCancel.price);

		if (priceLevelIt != bids.end()) {
			priceLevelIt->second.erase(listIt);
			if (priceLevelIt->second.empty()) {
				bids.erase(priceLevelIt);
			}
		}
	}
	else {
		auto priceLevelIt = asks.find(orderToCancel.price);

		if (priceLevelIt != asks.end()) {
			priceLevelIt->second.erase(listIt);
			if (priceLevelIt->second.empty()) {
				asks.erase(priceLevelIt);
			}
		}
	}

	orderLookup.erase(mapIt);
	return true;
}

void LimitOrderBook::addLimitOrder(Order incomingOrder)
{
	if (incomingOrder.side == Side::BUY) {
		auto priceLevelIt = bids.find(incomingOrder.price);

		if (priceLevelIt == bids.end()) {
			auto result = bids.emplace(incomingOrder.price, std::list<Order>{});
			priceLevelIt = result.first;
		}

		std::list<Order>::iterator newOrderIt = priceLevelIt->second.insert(
			priceLevelIt->second.end(),
			std::move(incomingOrder)
		);
		orderLookup.emplace(newOrderIt->id, newOrderIt);
	}
	else
	{
		auto priceLevelIt = asks.find(incomingOrder.price);

		if (priceLevelIt == asks.end()) {
			auto result = asks.emplace(incomingOrder.price, std::list<Order>{});
			priceLevelIt = result.first;
		}

		std::list<Order>::iterator newOrderIt = priceLevelIt->second.insert(
			priceLevelIt->second.end(),
			std::move(incomingOrder)
		);
		orderLookup.emplace(newOrderIt->id, newOrderIt);
	}
}

void LimitOrderBook::recordTrade(const Order& bidOrder, const Order& askOrder, long volume, double price)
{
	TradeRecord tradeRecord;

	tradeRecord.buyerOrderId = bidOrder.id;
	tradeRecord.sellerOrderId = askOrder.id;

	//Add timestamps
	tradeRecord.timeStamp = 0;

	tradeRecord.tradeId = nextTradeId++;
	tradeRecord.price = price;
	tradeRecord.volume = volume;

	tradeRecords.push_back(tradeRecord);
}