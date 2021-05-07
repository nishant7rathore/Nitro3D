#include "ResourceManager.h"
#include <queue>
#include <BWAPI.h>
#include "BuildingStrategyManager.h"

#pragma once
//AStar Node
struct AStarNode
{
    BWAPI::WalkPosition walkPos;
    //double distance;
    double gCost;
    double hCost;
    AStarNode* parent;

    AStarNode() 
    {
        this->walkPos = BWAPI::WalkPositions::Invalid;
        this->gCost = DBL_MAX;
        this->hCost = DBL_MAX;
        this->parent = nullptr;
    };

    AStarNode(BWAPI::WalkPosition tilePos, AStarNode* parent, double gCost, double hCost)
    {
        this->walkPos = tilePos;
        //this->distance = position;
        this->parent = parent;
        this->gCost = gCost;
        this->hCost = hCost;
    }

};

class NodeCostComparion
{

public:

    NodeCostComparion() {};

    bool operator() (const AStarNode& n1, const AStarNode& n2) const
    {
        return (n1.gCost + n1.hCost) > (n2.gCost + n2.hCost);
    }
};

class ResourceDistanceComparision
{

public:

    ResourceDistanceComparision() {};

    bool operator() (const Resource& r1, const Resource& r2) const
    {
        return r1.m_distance > r2.m_distance;
    }
};

struct ResourcePriorityQueue : std::priority_queue < Resource, std::vector<Resource>, ResourceDistanceComparision>
{
    std::vector<Resource> m_allMinerals;
    auto begin() const { return m_allMinerals.begin(); }
    auto end() const { return m_allMinerals.end(); }
};

class AStarPathFinding
{
    AStarNode m_startNode;
    AStarNode m_goalNode;

    double m_actionCost[8] = {8,8,8,8,11.314,11.314,11.314,11.314};

public:

    AStarPathFinding() {};
    AStarPathFinding(std::vector<Resource>& resources);
    std::priority_queue<AStarNode,std::vector<AStarNode>,NodeCostComparion> m_openList;
    Grid<int> m_closedList;
    Grid<AStarNode> m_openListGrid;
    double estimateCost(AStarNode n1, AStarNode n2);
    int startSearch(BWAPI::WalkPosition& startPos, BWAPI::WalkPosition& goalPos, BuildingStrategyManager& bm, Grid<int>& walkable, Grid<int>& buildable);
    ResourcePriorityQueue m_baseLocations;
};
