#include "BuildingStrategyManager.h"

BuildingStrategyManager::BuildingStrategyManager()
{
}

//BFS Node
struct BFSNode
{
    int x;
    int y;
    int position;
    BFSNode* parent;

    BFSNode(int x, int y, int position, BFSNode* parent)
    {
        this->x = x;
        this->y = y;
        this->position = position;
        this->parent = parent;
    }
};

//Direction struct for BFS
struct Direction
{
    int x = 0;
    int y = 0;
};

struct Direction directions[4] = { {1,0},{0,-1},{-1,0},{0,1} }; // legal Directions for BFS 

std::vector<BFSNode> openList; // BFS open list
std::map<std::string, bool> closedList; // BFS closed map

std::map<std::string, bool>::iterator it; // iterator to loop over closed list
std::vector<BFSNode> path; // final path 


BWAPI::TilePosition BuildingStrategyManager::getBuildingLocation(BWAPI::UnitType building)
{
    BWAPI::TilePosition& pos = BWAPI::Broodwar->self()->getStartLocation();

    openList.push_back(BFSNode(pos.x, pos.y, 0, nullptr));

    size_t size = openList.size();

    for (size_t i = 0; i < size; i++)
    {
        BFSNode node = openList[i];

        it = closedList.find(std::to_string(node.x) + std::to_string(node.y));
        if (it == closedList.end()) closedList.emplace(std::to_string(node.x) + std::to_string(node.y), true);

        for (size_t d = 0; d < 4; d++)
        {
            
            int x = node.x + directions[d].x;
            int y = node.y + directions[d].y;

            it = closedList.find(std::to_string(x) + std::to_string(y));

            if (BWAPI::Broodwar->canBuildHere(BWAPI::TilePosition(x,y), building))
            {
                return BWAPI::TilePosition(x, y);
            }

            if (it == closedList.end())
            {
                openList.push_back(BFSNode(x,y,node.position++,&node));
                size++;
            }

        }

    }
}

int BuildingStrategyManager::getNumberOfBuildings(BWAPI::UnitType building)
{
	return 0;
}
