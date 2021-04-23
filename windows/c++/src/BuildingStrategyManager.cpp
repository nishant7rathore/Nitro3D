#include "BuildingStrategyManager.h"
#include "MapTools.h"
#include <ctime>
#include <time.h>
#include <chrono>


// Sample Lehmer LCG PRNG Function (from slides)
// @author: David Churchill
inline uint32_t LehmerPRNG(uint32_t seed)
{
    uint64_t tmp;
    tmp = (uint64_t)seed * 0x4a39b70d;
    uint32_t m1 = (uint32_t)((tmp >> 32) ^ tmp);
    tmp = (uint64_t)m1 * 0x12fad5c9;
    uint32_t m2 = (uint32_t)((tmp >> 32) ^ tmp);
    return m2;
}

inline uint32_t Lehmer2(uint32_t x, uint32_t y)
{
    return LehmerPRNG(LehmerPRNG(x) ^ y);
}

BuildingStrategyManager::BuildingStrategyManager()
{

    m_vecBuildOrder.clear();

    //m_vecBuildOrder = std::vector<BWAPI::UnitType>(7, 0);

    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Forge);
    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Photon_Cannon);
    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Citadel_of_Adun);
    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Templar_Archives);
    //m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Assimilator);

    //m_vecBuildOrder[0] = BWAPI::UnitTypes::Protoss_Gateway;
    //m_vecBuildOrder[1] = BWAPI::UnitTypes::Protoss_Forge;
    //m_vecBuildOrder[2] = BWAPI::UnitTypes::Protoss_Photon_Cannon;
    //m_vecBuildOrder[3] = BWAPI::UnitTypes::Protoss_Assimilator;
    //m_vecBuildOrder[4] = BWAPI::UnitTypes::Protoss_Cybernetics_Core;
    //m_vecBuildOrder[5] = BWAPI::UnitTypes::Protoss_Citadel_of_Adun;
    //m_vecBuildOrder[6] = BWAPI::UnitTypes::Protoss_Templar_Archives;

    //m_vecBuildOrder = {BWAPI::UnitTypes::Protoss_Gateway, BWAPI::UnitTypes::Protoss_Forge , BWAPI::UnitTypes::Protoss_Photon_Cannon, BWAPI::UnitTypes::Protoss_Cybernetics_Core,BWAPI::UnitTypes::Protoss_Citadel_of_Adun,BWAPI::UnitTypes::Protoss_Templar_Archives,BWAPI::UnitTypes::Protoss_Assimilator };

    this->m_lastBuiltLocationMap.emplace(0, BWAPI::Broodwar->self()->getStartLocation());

    this->m_buildingBuidOrder.clear();
    this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Gateway, 2);
    //this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Nexus, 1);
    this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Forge, 1);
    this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Photon_Cannon, 5);
    this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 1);
    this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, 1);
    this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Templar_Archives, 1);
    this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Assimilator, 1);
    //this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Fleet_Beacon, 1);
    //this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Robotics_Facility, 1);
    //this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Observatory, 1);

    //this->m_buildingBuidOrder.emplace(BWAPI::UnitTypes::Protoss_Stargate, 1);

    this->m_additionalBaseBuildingMap.clear();
    this->m_additionalBaseBuildingMap.emplace(BWAPI::UnitTypes::Protoss_Pylon, 2);
    this->m_additionalBaseBuildingMap.emplace(BWAPI::UnitTypes::Protoss_Photon_Cannon, 6);
    this->m_additionalBaseBuildingMap.emplace(BWAPI::UnitTypes::Protoss_Gateway, 2);
    this->m_additionalBaseBuildingMap.emplace(BWAPI::UnitTypes::Protoss_Assimilator, 2);

    this->m_cannonLocations.clear();
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

struct Direction directions[8] = { {1,0},{0,1},{-1,0},{0,-1},{1,-1},{-1,1},{1,1},{-1,-1} }; // legal Directions for BFS 

std::vector<BFSNode> openList; // BFS open list
std::map<std::string, bool> closedList; // BFS closed map

std::map<std::string, bool>::iterator it; // iterator to loop over closed list
std::vector<BFSNode> path; // final path 


BWAPI::TilePosition& BuildingStrategyManager::getLastBuiltLocation(int base)
{
    return m_lastBuiltLocationMap[base];
}


BWAPI::TilePosition BuildingStrategyManager::getBuildingLocation(BWAPI::UnitType building, BWAPI::TilePosition pos)
{
    BWAPI::TilePosition lastBuiltPos = m_lastBuiltLocationMap[0];
    m_lastBuiltLocationMap[0] = pos;
    BWAPI::TilePosition retPos = getBuildingLocation(building,nullptr,0);
    m_lastBuiltLocationMap[0] = lastBuiltPos;

    return retPos;
}

BWAPI::TilePosition BuildingStrategyManager::getBuildingLocation(BWAPI::UnitType building, BWAPI::Unit builder, int base)
{

    openList.clear();
    closedList.clear();

    if (building == BWAPI::UnitTypes::Protoss_Pylon || building == BWAPI::UnitTypes::Protoss_Photon_Cannon)
    {
        BWAPI::TilePosition tempPos = getCannonPosition(base, building);
        if (tempPos.isValid())
        {
            return tempPos;
        }
    }

    BWAPI::TilePosition& lastBuiltLocation = m_lastBuiltLocationMap[base];
    openList.push_back(BFSNode(lastBuiltLocation.x, lastBuiltLocation.y, 0, nullptr));
    size_t size = openList.size();

    for (size_t i = 0; i < size; i++)
    {
        BFSNode node = openList[i];

        it = closedList.find(std::to_string(node.x) + std::to_string(node.y));
        if (it != closedList.end()) continue;
            
        closedList.emplace(std::to_string(node.x) + std::to_string(node.y), true);

        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        std::chrono::system_clock::duration dtn = tp.time_since_epoch();
        srand(dtn.count());
        int seed = rand();

        for (size_t d = 0; d < 8; d++)
        {
            size_t index = Lehmer2(seed, d) % 8;

            int x = node.x + directions[index].x;
            int y = node.y + directions[index].y;

            it = closedList.find(std::to_string(x) + std::to_string(y));

            BWAPI::TilePosition childPos = BWAPI::TilePosition(x, y);

            if (childPos.isValid() && BWAPI::Broodwar->canBuildHere(childPos, building))
            {
                bool isSafe = isSafeToPlaceHere(building,childPos);
  
                if (isSafe || building.isRefinery())
                {
                    lastBuiltLocation.x = childPos.x;
                    lastBuiltLocation.y = childPos.y;
                    return lastBuiltLocation;
                }
            }

            if (childPos.isValid() && it == closedList.end())
            {
                openList.push_back(BFSNode(x,y,node.position++,&node));
                size++;
            }

        }

    }
    return BWAPI::TilePositions::None;
}


bool BuildingStrategyManager::isSafeToPlaceHere(BWAPI::UnitType building, BWAPI::TilePosition childPos)
{
    bool isSafe = true;

    for (int tileX = 0; tileX < building.tileWidth(); tileX++)
    {
        for (int tileY = 0; tileY < building.tileHeight(); tileY++)
        {
            if (BWAPI::Broodwar->getUnitsOnTile(childPos.x + tileX, childPos.y + tileY).size() > 0)
            {
                isSafe = false;
            }
        }
    }

    return isSafe && BWAPI::Broodwar->canBuildHere(childPos, building);
}

int BuildingStrategyManager::getNumberOfBuildings(BWAPI::UnitType building)
{
    return m_buildingBuidOrder[building];
}

int BuildingStrategyManager::getSecondaryBaseNumberOfBuildings(BWAPI::UnitType building)
{
    return m_additionalBaseBuildingMap[building];
}

std::map<BWAPI::UnitType, int> BuildingStrategyManager::getBuildingOrderMap()
{
    return m_buildingBuidOrder;
}

std::vector<BWAPI::UnitType>& BuildingStrategyManager::getBuildingOrderVector()
{
    return m_vecBuildOrder;
}

std::map<BWAPI::UnitType, int> BuildingStrategyManager::getAdditionalBaseBuildingOrderMap()
{
    return m_additionalBaseBuildingMap;
}

int& BuildingStrategyManager::getWorkerID()
{
    return m_workerID;
}

bool& BuildingStrategyManager::isAdditionalSupplyNeeded()
{
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    m_isAdditionalSupplyNeeded = unusedSupply <= 8;

    return m_isAdditionalSupplyNeeded;
}

bool& BuildingStrategyManager::isBuildingBuiltNeeded()
{
    return m_isBuildingBuiltNeeded;
}


void BuildingStrategyManager::findCannonBuildingLocation(int base, Grid<int>& walkable, Grid<int>& buildable)
{

    openList.clear();
    closedList.clear();

    BWAPI::TilePosition& lastBuiltLocation = m_lastBuiltLocationMap[base];
    openList.push_back(BFSNode(lastBuiltLocation.x, lastBuiltLocation.y, 0, nullptr));
    size_t size = openList.size();

    for (size_t i = 0; i < size; i++)
    {
        BFSNode node = openList[i];

        it = closedList.find(std::to_string(node.x) + std::to_string(node.y));
        
        if (it != closedList.end()) continue;

        closedList.emplace(std::to_string(node.x) + std::to_string(node.y), true);
   
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        std::chrono::system_clock::duration dtn = tp.time_since_epoch();
        srand(dtn.count());
        int seed = rand();

        for (size_t d = 0; d < 8; d++)
        {
            size_t index = Lehmer2(seed, d) % 8;

            int x = node.x + directions[index].x;
            int y = node.y + directions[index].y;

            it = closedList.find(std::to_string(x) + std::to_string(y));

            BWAPI::TilePosition childPos = BWAPI::TilePosition(x, y);
            if (!childPos.isValid() || it != closedList.end()) continue;

            bool isBuildable = buildable.get(x, y);

            bool isSafeToPlace = isSafeToPlaceHere(BWAPI::UnitTypes::Protoss_Photon_Cannon, childPos);

            if (isBuildable)
            {
               /* bool isSafe = isSafeToPlaceHere(building, childPos);

                if (isSafe || building.isRefinery() || building == BWAPI::UnitTypes::Protoss_Pylon)
                {
                    lastBuiltLocation.x = childPos.x;
                    lastBuiltLocation.y = childPos.y;
                    return lastBuiltLocation;
                }*/

                bool isNeighborNotVisible = true;

                BWAPI::TilePosition newChildPos = BWAPI::TilePosition(x, y);

                seed = rand();

                for (size_t dd = 0; dd < 8; dd++)
                {
                    size_t ind = Lehmer2(seed, dd) % 8;

                    int xx = childPos.x + directions[ind].x;
                    int yy = childPos.y + directions[ind].y;

                    newChildPos = BWAPI::TilePosition(xx,yy);
                    if (newChildPos.isValid() && !BWAPI::Broodwar->isVisible(newChildPos) && walkable.get(xx,yy))
                    {
                        isNeighborNotVisible = false;
                        break;
                    }
                }
                if (!isNeighborNotVisible)
                {
                    this->m_cannonLocations[base].push_back(childPos);
                    if (this->m_cannonLocations[base].size() >= 100)
                    {
                        return;
                    }
                }
            }

            openList.push_back(BFSNode(childPos.x, childPos.y, node.position++, &node));
            size++;

        }

    }
  
}

BWAPI::TilePosition BuildingStrategyManager::getCannonPosition(int base, BWAPI::UnitType unitType)
{
    for (auto& pos: m_cannonLocations[base])
    {
        if (isSafeToPlaceHere(unitType,pos) && BWAPI::Broodwar->canBuildHere(pos,unitType,nullptr,true))
        {
            return pos;
        }
    }
    return BWAPI::TilePositions::Invalid;
}
