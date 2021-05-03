#include "MapTools.h"
#include "ResourceManager.h"
#include <queue>


#pragma once
//AStar Node
struct AStarNode
{
    BWAPI::TilePosition tilePos;
    //double distance;
    double gCost;
    double hCost;
    AStarNode* parent;

    AStarNode() 
    {
        this->tilePos = BWAPI::TilePositions::Invalid;
        this->gCost = DBL_MAX;
        this->hCost = DBL_MAX;
        this->parent = nullptr;
    };

    AStarNode(BWAPI::TilePosition tilePos, AStarNode* parent, double gCost, double hCost)
    {
        this->tilePos = tilePos;
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

class AStarPathFinding
{
    AStarNode m_startNode;
    AStarNode m_goalNode;

    int m_actionCost[8] = {32,32,32,32,32,32,32,32};

public:

    AStarPathFinding() {};
    //AStarPathFinding(AStarNode startNode, AStarNode goalNode)
    //{
    //    m_startNode = startNode;
    //    m_goalNode = goalNode;
    //}
    std::priority_queue<AStarNode,std::vector<AStarNode>,NodeCostComparion> m_openList;
    std::map<std::string,bool> m_closedList;
    Grid<AStarNode> m_openListGrid;
    double estimateCost(AStarNode n1, AStarNode n2);
    int startSearch(BWAPI::TilePosition& startPos, BWAPI::TilePosition& goalPos, BuildingStrategyManager& bm, Grid<int>& walkable, Grid<int>& buildable);

};
