#pragma once
#include "map"
#include "vector"
#include "BWAPI.h"
#include "Grid.hpp"

class BuildingStrategyManager
{
	std::map<int,BWAPI::TilePosition> m_lastBuiltLocationMap;
	std::map<BWAPI::UnitType, int> m_additionalBaseBuildingMap;
	std::map<int, std::vector<BWAPI::TilePosition>> m_cannonLocations;
	int m_workerID = -1;
	bool m_isAdditionalSupplyNeeded = false;
	bool m_isBuildingBuiltNeeded = false;

public:
	BuildingStrategyManager();

	std::map<BWAPI::UnitType,int> m_buildingBuidOrder;
	BWAPI::TilePosition& getLastBuiltLocation(int base = 0);
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building, BWAPI::TilePosition pos);
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building, BWAPI::Unit builder, int base = 0);
	int getNumberOfBuildings(BWAPI::UnitType building);
	int getSecondaryBaseNumberOfBuildings(BWAPI::UnitType building);
	bool isSafeToPlaceHere(BWAPI::UnitType building, BWAPI::TilePosition childPos);
	std::map<BWAPI::UnitType, int> getBuildingOrderMap();
	std::map<BWAPI::UnitType, int> getAdditionalBaseBuildingOrderMap();
	int& getWorkerID();
	bool& isAdditionalSupplyNeeded();
	bool& isBuildingBuiltNeeded();
	void findCannonBuildingLocation(int base, Grid<int>& walkable, Grid<int>& buildable);
	BWAPI::TilePosition getCannonPosition(int base, BWAPI::UnitType unitType);
};

