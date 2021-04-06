#pragma once
#include "map"
#include "BWAPI.h"
class BuildingStrategyManager
{
	BWAPI::TilePosition m_lastBuiltLocation;

public:
	BuildingStrategyManager();

	std::map<BWAPI::UnitType,int> m_buildingBuidOrder;
	BWAPI::TilePosition& getLastBuiltLocation();
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building, BWAPI::Unit builder, BWAPI::TilePosition lastBuiltLocation);
	int getNumberOfBuildings(BWAPI::UnitType building);
	std::map<BWAPI::UnitType, int> getBuildingOrderMap();
};

