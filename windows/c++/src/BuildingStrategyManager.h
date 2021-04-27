#pragma once
#include "map"
#include "vector"
#include "BWAPI.h"
#include "Grid.hpp"

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



class BuildingStrategyManager
{
	Grid<int>& m_walkable;
	Grid<int>& m_buildable;
	std::map<int,BWAPI::TilePosition> m_lastBuiltLocationMap;
	std::map<BWAPI::UnitType, int> m_additionalBaseBuildingMap;
	std::map<int, std::vector<BWAPI::TilePosition>> m_cannonLocations;
	std::vector<BWAPI::UnitType> m_vecBuildOrder;
	int m_workerID = -1;
	bool m_isAdditionalSupplyNeeded = false;
	bool m_isBuildingBuiltNeeded = false;
	int m_DirectionIndices[8] = {0,1,2,3,4,5,6,7};

	std::vector<BFSNode> openList; // BFS open list
	std::map<std::string, bool> closedList; // BFS closed map

	std::map<std::string, bool>::iterator it; // iterator to loop over closed list
	std::vector<BFSNode> path; // final path 

public:
	BuildingStrategyManager();
	BuildingStrategyManager(Grid<int>& walkable, Grid<int>& buildable);

	struct Direction m_directions[8] = { {1,0},{0,1},{-1,0},{0,-1},{1,-1},{-1,1},{1,1},{-1,-1} }; // legal Directions for BFS 

	void randomizeArray(int seed);

	std::map<BWAPI::UnitType,int> m_buildingBuidOrder;
	BWAPI::TilePosition& getLastBuiltLocation(int base = 0);
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building, BWAPI::TilePosition pos);
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building, BWAPI::Unit builder, int base = 0);
	int getNumberOfBuildings(BWAPI::UnitType building);
	int getSecondaryBaseNumberOfBuildings(BWAPI::UnitType building);
	bool isSafeToPlaceHere(BWAPI::UnitType building, BWAPI::TilePosition childPos);
	std::map<BWAPI::UnitType, int> getBuildingOrderMap();
	std::vector<BWAPI::UnitType>& getBuildingOrderVector();
	std::map<BWAPI::UnitType, int> getAdditionalBaseBuildingOrderMap();
	int& getWorkerID();
	bool& isAdditionalSupplyNeeded();
	bool& isBuildingBuiltNeeded();
	void setGrids(Grid<int>& walkable, Grid<int>& buildable);
	void findCannonBuildingLocation(int base);
	BWAPI::TilePosition getCannonPosition(int base, BWAPI::UnitType unitType);
};



