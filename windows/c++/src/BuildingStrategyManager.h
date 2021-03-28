#pragma once
#include "map"
#include "BWAPI.h"
class BuildingStrategyManager
{
public:
	BuildingStrategyManager();

	std::map<BWAPI::UnitType,int> m_buildingBuidOrder;
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building);
	int getNumberOfBuildings(BWAPI::UnitType building);
};

