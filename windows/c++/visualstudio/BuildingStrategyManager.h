#pragma once
#include "StrategyManager.h"
class BuildingStrategyManager : public StrategyManager
{
public:
	BuildingStrategyManager();
	BWAPI::TilePosition getBuildingLocation(BWAPI::UnitType building);
	int getNumberOfBuildings(BWAPI::UnitType building);
};

