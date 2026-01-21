#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

#include "datatypes.h"
#include "LimitOrderBook.h"
#include "DepthChart.h"
#include "UIHelpers.h"

DepthChart::DepthChart() {
    bidTriangles.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    askTriangles.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
}

void DepthChart::update(const LimitOrderBook& LOB, float chartWidth, float chartHeight, sf::Vector2u winSize) {
    long totalVolume = 0;
    float binSize = 0.5f;

    std::vector<DepthPoint> depthPoints = LOB.depthChartPoints(binSize, &totalVolume);
    if (depthPoints.empty()) return;

    float binWidth = chartWidth / static_cast<float>(depthPoints.size() - 1);
    float bottomOfChart = winSize.y;

    size_t midIndex = depthPoints.size() / 2;

    size_t askCount = depthPoints.size() - midIndex;

    bidTriangles.resize((midIndex + 1) * 2);
    askTriangles.resize(askCount * 2);

    for (size_t i = 0; i <= midIndex; i++)
    {
        auto& curPoint = depthPoints[i];
        float hPerc = (totalVolume > 0) ? static_cast<float>(curPoint.totalVolume) / totalVolume : 0.f;

        float xPos = (winSize.x - chartWidth) + i * binWidth;
        float yPeak = bottomOfChart - (hPerc * chartHeight);

        bidTriangles[2 * i].position = { xPos, yPeak };
        bidTriangles[2 * i + 1].position = { xPos, bottomOfChart };

        sf::Color col = (curPoint.totalVolume == 0) ? Theme::TextDim : Theme::Bid;
        sf::Color col2 = (curPoint.totalVolume == 0) ? Theme::TextDim : Theme::BidBG;
        bidTriangles[2 * i].color = col;
        bidTriangles[2 * i + 1].color = col2;
    }

    for (size_t i = 0; i < askCount; i++)
    {
        size_t dataIdx = midIndex + i;
        auto& curPoint = depthPoints[dataIdx];
        float hPerc = (totalVolume > 0) ? static_cast<float>(curPoint.totalVolume) / totalVolume : 0.f;

        float xPos = (winSize.x - chartWidth) + dataIdx * binWidth;
        float yPeak = bottomOfChart - (hPerc * chartHeight);

        askTriangles[2 * i].position = { xPos, yPeak };
        askTriangles[2 * i + 1].position = { xPos, bottomOfChart };

        sf::Color col = (curPoint.totalVolume == 0) ? Theme::TextDim : Theme::Ask;
        sf::Color col2 = (curPoint.totalVolume == 0) ? Theme::TextDim : Theme::AskBG;
        askTriangles[2 * i].color = col;
        askTriangles[2 * i + 1].color = col2;
    }
}

void DepthChart::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(bidTriangles, states);
    target.draw(askTriangles, states);
}