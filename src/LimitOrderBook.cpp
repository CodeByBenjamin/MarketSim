#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <string>
#include <chrono>

#include "datatypes.h"
#include "UIHelpers.h"
#include "LimitOrderBook.h"

const std::map<double, std::list<Order>, std::greater<double>>& LimitOrderBook::getBids() const
{
	return bids;
}

const std::map<double, std::list<Order>>& LimitOrderBook::getAsks() const
{
	return asks;
}

long LimitOrderBook::getHighestVolume(Side side, size_t priceLevels) const
{
	long onePriceVol = 0;
	long maxVol = 0;

	int count = 0;

	if (bids.empty() || asks.empty())
	{
		return 0;
	}

	if (side == Side::BUY)
	{
		for (auto it = bids.begin(); it != bids.end() && count < priceLevels; ++it, ++count) {
			onePriceVol = 0;
			for (const auto& order : it->second) onePriceVol += order.volume;
			if (onePriceVol > maxVol) maxVol = onePriceVol;
		}
	}
	else
	{
		for (auto it = asks.begin(); it != asks.end() && count < priceLevels; ++it, ++count) {
			onePriceVol = 0;
			for (const auto& order : it->second) onePriceVol += order.volume;
			if (onePriceVol > maxVol) maxVol = onePriceVol;
		}
	}

	return maxVol;
}

void LimitOrderBook::update()
{
	double midPrice = (bids.begin()->first + asks.begin()->first) / 2.0;
	midPriceRecords.push_back(midPrice);
}

const std::vector<TradeRecord>& LimitOrderBook::getTradeHistory() const
{
	return tradeRecords;
}

const std::vector<double>& LimitOrderBook::getMidPriceHistory() const
{
	return midPriceRecords;
}

void LimitOrderBook::processOrder(const Order& incomingOrder)
{
	Order order = incomingOrder;
	order.id = nextOrderId++;

	if (order.side == Side::BUY) {
		if (!asks.empty() && order.price >= asks.begin()->first)
			executeMatch(order);
		else
			addLimitOrder(order);
	}
	else
	{
		if (!bids.empty() && order.price <= bids.begin()->first)
			executeMatch(order);
		else
			addLimitOrder(order);
	}
}

void LimitOrderBook::executeMatch(Order& incomingOrder) 
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

			if (priceList.empty())
				priceLevelIt = asks.erase(priceLevelIt);
			else
				break;
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

			if (priceList.empty())
				priceLevelIt = bids.erase(priceLevelIt);
			else
				break;
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
	TradeRecord tradeRecord = {};

	tradeRecord.buyerOrderId = bidOrder.id;
	tradeRecord.sellerOrderId = askOrder.id;

	//auto now = std::chrono::system_clock::now();
	//tradeRecord.timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(
	//	now.time_since_epoch()
	//).count();

	tradeRecord.tradeId = nextTradeId++;
	tradeRecord.price = price;
	tradeRecord.volume = volume;

	tradeRecords.push_back(tradeRecord);
}

const std::vector<DepthPoint> LimitOrderBook::depthChartPoints(float binSize, long* totalVolume) const
{
	std::vector<DepthPoint> depthPoints;
	if (bids.empty() || asks.empty()) { *totalVolume = 0; return depthPoints; }

	depthPoints.reserve(bids.size() + asks.size() + 1);

	long runningBidVol = 0;
	std::vector<DepthPoint> tempBids;

	//Adding points in reverse then reversing to avoid adding to front
	for (auto it = bids.begin(); it != bids.end(); ++it) {
		long levelVol = 0;
		for (const auto& order : it->second) levelVol += order.volume;

		runningBidVol += levelVol;
		float priceLevel = std::floor(it->first / binSize) * binSize;

		tempBids.push_back({ priceLevel, runningBidVol });
	}

	for (auto it = tempBids.rbegin(); it != tempBids.rend(); ++it) {
		depthPoints.push_back(*it);
	}

	//Midpoint
	float midPrice = (bids.begin()->first + asks.begin()->first) / 2.0f;
	depthPoints.push_back({ std::floor(midPrice / binSize) * binSize, 0 });

	long runningAskVol = 0;
	for (auto it = asks.begin(); it != asks.end(); ++it) {
		long levelVol = 0;
		for (const auto& order : it->second) levelVol += order.volume;
		runningAskVol += levelVol;

		float priceLevel = std::floor(it->first / binSize) * binSize;
		depthPoints.push_back({ priceLevel, runningAskVol });
	}

	*totalVolume = std::max(runningBidVol, runningAskVol);
	return depthPoints;
}