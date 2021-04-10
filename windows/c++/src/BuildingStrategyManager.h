#pragma once
#include "map"
#include "BWAPI.h"
class BuildingStrategyManager
{
	BWAPI::TilePosition m_lastBuiltLocation;
	int m_workerID = -1;
	bool m_isAdditionalSupplyNeeded = false;

public:
	BuildingStrategyManager();

	std::map<BWAPI::UnitType,int> m_buildingBuidOrder;
	BWAPI::TilePosition& getLastBuiltLocation();
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building, BWAPI::Unit builder, BWAPI::TilePosition lastBuiltLocation);
	int getNumberOfBuildings(BWAPI::UnitType building);
	bool isSafeToPlaceHere(BWAPI::UnitType building, BWAPI::TilePosition childPos);
	std::map<BWAPI::UnitType, int> getBuildingOrderMap();
	int& getWorkerID();
	bool& isAdditionalSupplyNeeded();
};

