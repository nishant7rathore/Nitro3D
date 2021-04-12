#pragma once
#include "map"
#include "BWAPI.h"
class BuildingStrategyManager
{
	std::map<int,BWAPI::TilePosition> m_lastBuiltLocationMap;
	int m_workerID = -1;
	bool m_isAdditionalSupplyNeeded = false;

public:
	BuildingStrategyManager();

	std::map<BWAPI::UnitType,int> m_buildingBuidOrder;
	BWAPI::TilePosition& getLastBuiltLocation(int base = 0);
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building, BWAPI::Unit builder, int base = 0);
	int getNumberOfBuildings(BWAPI::UnitType building);
	bool isSafeToPlaceHere(BWAPI::UnitType building, BWAPI::TilePosition childPos);
	std::map<BWAPI::UnitType, int> getBuildingOrderMap();
	int& getWorkerID();
	bool& isAdditionalSupplyNeeded();
};

