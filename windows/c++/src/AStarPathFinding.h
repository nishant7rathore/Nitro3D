#include "MapTools.h"
#include "ResourceManager.h"
#include <queue>


#pragma once
//AStar Node
struct AStarNode
{
    BWAPI::TilePosition tilePos;
    double distance;
    double gCost;
    double hCost;
    AStarNode* parent;

    AStarNode() {};

    AStarNode(BWAPI::TilePosition tilePos, double position, AStarNode* parent, double gCost, double hCost)
    {
        this->tilePos = tilePos;
        this->distance = position;
        this->parent = parent;
        this->gCost = gCost;
        this->hCost = hCost;
    }

};



class AStar
{
    AStarNode startNode;
    AStarNode goalNode;

public:

    AStar() {};
    std::priority_queue<double> m_fnQueue;
    double estimateCost(AStarNode n1, AStarNode n2);
    void startSearch(BWAPI::TilePosition& startPos, std::vector<Resource>& vespeneGeyserList, BuildingStrategyManager& bm, Grid<int>& walkable, Grid<int>& buildable);

};
