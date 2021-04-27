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

BuildingStrategyManager::BuildingStrategyManager(): m_walkable(Grid<int>()), m_buildable(Grid<int>())
{

}

BuildingStrategyManager::BuildingStrategyManager(Grid<int>& walkable, Grid<int>& buildable): m_walkable(walkable),m_buildable(buildable)
{
    m_vecBuildOrder.clear();

    //m_vecBuildOrder = std::vector<BWAPI::UnitType>(7, 0);

    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Gateway);
    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Forge);
    m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Photon_Cannon);
    //m_vecBuildOrder.push_back(BWAPI::UnitTypes::Protoss_Assimilator);
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
    this->m_additionalBaseBuildingMap.emplace(BWAPI::UnitTypes::Protoss_Photon_Cannon, 5);
    this->m_additionalBaseBuildingMap.emplace(BWAPI::UnitTypes::Protoss_Gateway, 2);
    this->m_additionalBaseBuildingMap.emplace(BWAPI::UnitTypes::Protoss_Assimilator, 1);

    this->m_cannonLocations.clear();
}

void BuildingStrategyManager::randomizeArray(int seed)
{
    //while ()
    //{
    //    size_t index = Lehmer2(seed, 0) % 8;
    //}
    
}




BWAPI::TilePosition& BuildingStrategyManager::getLastBuiltLocation(int base)
{
    return m_lastBuiltLocationMap[base];
}

// for new base nexus
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

    BWAPI::TilePosition& lastBuiltLocation = m_lastBuiltLocationMap[base];

    if (BWAPI::Broodwar->canBuildHere(lastBuiltLocation, building))
    {
        return lastBuiltLocation;
    }

    //if (building == BWAPI::UnitTypes::Protoss_Pylon || building == BWAPI::UnitTypes::Protoss_Photon_Cannon)
    //{
    //    BWAPI::TilePosition tempPos = getCannonPosition(base, building);
    //    if (tempPos.isValid())
    //    {
    //        m_lastBuiltLocationMap[base] = tempPos;
    //        return tempPos;
    //    }
    //    else
    //    {
    //        findCannonBuildingLocation(base);
    //        return tempPos;
    //    }
    //   
    //}

    //else if (building == BWAPI::UnitTypes::Protoss_Assimilator)
    //{
    //    BWAPI::TilePosition tempPos = BWAPI::Broodwar->getBuildLocation(building, m_lastBuiltLocationMap[base]);
    //    if (tempPos.isValid())
    //    {
    //        m_lastBuiltLocationMap[base] = tempPos;
    //    }
    //}


    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    std::chrono::system_clock::duration dtn = tp.time_since_epoch();
    srand((unsigned int)dtn.count());

    openList.push_back(BFSNode(lastBuiltLocation.x, lastBuiltLocation.y, 0, nullptr));
    size_t size = openList.size();

    bool isNexus = building == BWAPI::UnitTypes::Protoss_Nexus;

    int seed = rand();

    for (size_t i = 0; i < size; i++)
    {
        BFSNode node = openList[i];

        it = closedList.find(std::to_string(node.x) + std::to_string(node.y));
        if (closedList[std::to_string(node.x) + std::to_string(node.y)]) continue;
            
        closedList.emplace(std::to_string(node.x) + std::to_string(node.y), true);

        for (size_t d = 0; d < 8; d++)
        {
            
            size_t index = Lehmer2(seed, d) % 8;

            int x = node.x + m_directions[d].x;
            int y = node.y + m_directions[d].y;

            it = closedList.find(std::to_string(x) + std::to_string(y));

            BWAPI::TilePosition childPos = BWAPI::TilePosition(x, y);

            if (childPos.isValid() && BWAPI::Broodwar->canBuildHere(childPos, building,nullptr, !isNexus))
            {
                bool isSafe = isSafeToPlaceHere(building,childPos);
  
                if (isSafe || building.isRefinery())
                {
                    lastBuiltLocation.x = childPos.x;
                    lastBuiltLocation.y = childPos.y;
                    return lastBuiltLocation;
                }
            }

            if (childPos.isValid() && closedList[std::to_string(childPos.x) + std::to_string(childPos.y)])
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

    for (int tileX = -1; tileX < building.tileWidth() + 1; tileX++)
    {
        for (int tileY = -1; tileY < building.tileHeight() + 1; tileY++)
        {
            if (!BWAPI::TilePosition(childPos.x + tileX, childPos.y + tileY).isValid() || BWAPI::Broodwar->getUnitsOnTile(childPos.x + tileX, childPos.y + tileY).size() > 0)
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

void BuildingStrategyManager::setGrids(Grid<int>& walkable, Grid<int>& buildable)
{
    m_walkable = walkable;
    m_buildable = buildable;
}


void BuildingStrategyManager::findCannonBuildingLocation(int base)
{
    this->m_cannonLocations[base].clear();
    openList.clear();
    closedList.clear();

    std::map<std::string, bool> closedList2;


    BWAPI::TilePosition& lastBuiltLocation = m_lastBuiltLocationMap[base];

    openList.push_back(BFSNode(lastBuiltLocation.x, lastBuiltLocation.y, 0, nullptr));

    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    std::chrono::system_clock::duration dtn = tp.time_since_epoch();
    srand((unsigned int)dtn.count());

    size_t size = openList.size();

    for (size_t i = 0; i < size; i++)
    {
        BFSNode node = openList[i];

        it = closedList.find(std::to_string(node.x) + std::to_string(node.y));
        
        if (it != closedList.end()) continue;

        closedList.emplace(std::to_string(node.x) + std::to_string(node.y), true);
  

        for (size_t d = 0; d < 8; d++)
        {
            int seed = rand();
            size_t index = Lehmer2(seed, d) % 8;

            int x = node.x + m_directions[d].x;
            int y = node.y + m_directions[d].y;

            it = closedList.find(std::to_string(x) + std::to_string(y));

            BWAPI::TilePosition childPos = BWAPI::TilePosition(x, y);
            if (!childPos.isValid() || it != closedList.end()) continue;

            bool isBuildable = m_buildable.get(x, y);

            //bool isSafeToPlace = isSafeToPlaceHere(BWAPI::UnitTypes::Protoss_Photon_Cannon, childPos);

            //closedList.emplace(std::to_string(childPos.x) + std::to_string(childPos.y), true);

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

                    int xx = childPos.x + m_directions[dd].x;
                    int yy = childPos.y + m_directions[dd].y;

                    newChildPos = BWAPI::TilePosition(xx,yy);
                    if (newChildPos.isValid() && !BWAPI::Broodwar->isVisible(newChildPos) && m_walkable.get(xx,yy))
                    {
                        isNeighborNotVisible = false;
                        break;
                    }
                }
                if (!isNeighborNotVisible)
                {
                    //it = closedList.find(std::to_string(newChildPos.x) + std::to_string(newChildPos.y));
                    //if (it == closedList.end()) 
                    //{
                    //    this->m_cannonLocations[base].push_back(childPos);

                    //    closedList.emplace(std::to_string(newChildPos.x) + std::to_string(newChildPos.y), true);
                    //    
                    //}

                    if (!closedList2[std::to_string(x) + std::to_string(y)])
                    {
                        if (isSafeToPlaceHere(BWAPI::UnitTypes::Protoss_Pylon, childPos) && BWAPI::Broodwar->canBuildHere(childPos, BWAPI::UnitTypes::Protoss_Pylon, nullptr, true))
                        {
                            this->m_cannonLocations[base].push_back(childPos);
                        }
                    }

                    if (this->m_cannonLocations[base].size() >= 1)
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
        if (isSafeToPlaceHere(BWAPI::UnitTypes::Protoss_Pylon, pos))
        {
            return pos;
        }
    }
    return BWAPI::TilePositions::Invalid;
}
