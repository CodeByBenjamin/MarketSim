#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

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
	TradeRecord tradeRecord = {};

	tradeRecord.buyerOrderId = bidOrder.id;
	tradeRecord.sellerOrderId = askOrder.id;

	//Add timestamps
	tradeRecord.timeStamp = 0;

	tradeRecord.tradeId = nextTradeId++;
	tradeRecord.price = price;
	tradeRecord.volume = volume;

	tradeRecords.push_back(tradeRecord);
}

const std::vector<DepthPoint> LimitOrderBook::depthChartPoints(float binSize, long *totalVolume) const
{
	std::vector<DepthPoint> depthPoints = std::vector<DepthPoint>{};

	long totalVolBids = 0;

	for (auto it = bids.begin(); it != bids.end(); ++it) {

		const auto& samePriceList = it->second;
		float priceLevel = std::floor(it->first / binSize) * binSize;

		for (auto it2 = samePriceList.begin(); it2 != samePriceList.end(); ++it2) {
			totalVolBids += it2->volume;
		}

		depthPoints.emplace(depthPoints.begin(), DepthPoint{ priceLevel, totalVolBids });
	}

	depthPoints.emplace_back(DepthPoint{ std::floor(static_cast<float>(bids.begin()->first + asks.begin()->first) / ( 2 * binSize)) * binSize, 0});

	long totalVolAsks = 0;

	for (auto it = asks.begin(); it != asks.end(); ++it) {

		const auto& samePriceList = it->second;
		float priceLevel = std::floor(it->first / binSize) * binSize;

		for (auto it2 = samePriceList.begin(); it2 != samePriceList.end(); ++it2) {
			totalVolAsks += it2->volume;
		}

		depthPoints.emplace_back(DepthPoint{ priceLevel, totalVolAsks });
	}

	*totalVolume = std::max(totalVolBids, totalVolAsks);

	return depthPoints;
}

void LimitOrderBook::draw(sf::RenderWindow& window, const sf::Font& font, float lobWidth)
{
	//Draw LOBPanel

	UIHelper::drawColoredRect(window, 0.f, 0.f, lobWidth, 1080.f, LEFT, 0.f, Theme::Surface);

	//Draw Label Text

	UIHelper::drawLabel(window, font, "BIDS ($)", 28, lobWidth / 4.f, 20.f, CENTER, 0.f, Theme::Bid);
	UIHelper::drawLabel(window, font, "ASKS ($)", 28, 3.f * lobWidth / 4.f, 20.f, CENTER, 0.f, Theme::Ask);

	float rowHeight = 30.f;
	float padding = 5.f;
	float currentY = 130.f;

	if (!bids.empty() && !asks.empty()) {
		double bestBid = bids.begin()->first;
		double bestAsk = asks.begin()->first;
		double mid = (bestBid + bestAsk) / 2.0;
		double spread = bestAsk - bestBid;

		float centerX = lobWidth / 2.f;

		std::string fullPrice = UIHelper::formatPrice(mid);
		size_t dotPos = fullPrice.find('.');

		std::string leftSide = fullPrice.substr(0, dotPos);
		std::string rightSide = fullPrice.substr(dotPos);
		UIHelper::drawLabel(window, font, leftSide, 30, centerX, 3.f * currentY / 4.f, RIGHT, -9.f, Theme::TextDim);
		UIHelper::drawLabel(window, font, rightSide, 30, centerX, 3.f * currentY / 4.f, LEFT, -9.f, Theme::TextDim);

		fullPrice = UIHelper::formatPrice(spread);
		dotPos = fullPrice.find('.');

		leftSide = fullPrice.substr(0, dotPos);
		rightSide = fullPrice.substr(dotPos);
		UIHelper::drawLabel(window, font, leftSide, 22, centerX, currentY / 2.f, RIGHT, -7.f, Theme::Accent);
		UIHelper::drawLabel(window, font, rightSide, 22, centerX, currentY / 2.f, LEFT, -7.f, Theme::Accent);
	}

	long maxVol = 1;
	int count = 0;
	int maxCount = 30;

	for (auto it = bids.begin(); it != bids.end() && count < maxCount; ++it, ++count) {

		const auto& samePriceList = it->second;
		long onePriceVol = 0;

		for (auto it2 = samePriceList.begin(); it2 != samePriceList.end(); ++it2) {
			onePriceVol += it2->volume;
		}

		if (onePriceVol > maxVol) maxVol = onePriceVol;
	}

	count = 0;
	for (auto it = asks.begin(); it != asks.end() && count < maxCount; ++it, ++count) {

		const auto& samePriceList = it->second;
		long onePriceVol = 0;

		for (auto it2 = samePriceList.begin(); it2 != samePriceList.end(); ++it2) {
			onePriceVol += it2->volume;
		}

		if (onePriceVol > maxVol) maxVol = onePriceVol;
	}

	count = 0;
	for (auto it = bids.begin(); it != bids.end() && count < maxCount; ++it, ++count) {
		long onePriceVol = 0;
		for (const auto& order : it->second) onePriceVol += order.volume;

		float fullPerc = static_cast<float>(onePriceVol) / maxVol;
		float yPos = currentY + (rowHeight + padding) * count;
		float centerX = lobWidth / 2.f;

		UIHelper::drawColoredRect(window, centerX, yPos, centerX * fullPerc, rowHeight, RIGHT, 0.f, Theme::BidBG);

		UIHelper::drawLabel(window, font, UIHelper::formatPrice(it->first), 24, centerX, yPos, RIGHT, -10.f, Theme::Bid);
		UIHelper::drawLabel(window, font, std::to_string(onePriceVol), 22, 0.f, yPos, LEFT, 10.f, Theme::TextDim);
	}

	count = 0;
	for (auto it = asks.begin(); it != asks.end() && count < maxCount; ++it, ++count) {
		long onePriceVol = 0;
		for (const auto& order : it->second) onePriceVol += order.volume;

		float fullPerc = static_cast<float>(onePriceVol) / maxVol;
		float yPos = currentY + (rowHeight + padding) * count;
		float centerX = lobWidth / 2.f;

		UIHelper::drawColoredRect(window, centerX, yPos, centerX * fullPerc, rowHeight, LEFT, 0.f, Theme::AskBG);

		UIHelper::drawLabel(window, font, UIHelper::formatPrice(it->first), 24, centerX, yPos, LEFT, 10.f, Theme::Ask);
		UIHelper::drawLabel(window, font, std::to_string(onePriceVol), 22, lobWidth, yPos, RIGHT, -10.f, Theme::TextDim);
	}

	UIHelper::drawColoredRect(window, lobWidth, 0.f, 2.f, window.getSize().y, LEFT, 0.f, Theme::Border);
	UIHelper::drawColoredRect(window, lobWidth / 2.f, currentY, 2.f, window.getSize().y, CENTER, 0.f, Theme::Border);
}