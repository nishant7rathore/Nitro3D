#pragma once
#include <BWAPI.h>
#include <map>
#include "BuildingStrategyManager.h"
#include "UnitStrategyManager.h"

class StrategyManager
{
	BWAPI::UnitCommand nextCommand;

	std::map<BWAPI::UnitType, int> m_unitTypesToTotal;

	BuildingStrategyManager m_buildingStrategyManager;
	UnitStrategyManager m_unitStrategyManager;

public:

	StrategyManager();

	//virtual BWAPI::UnitCommand getNextCommand();
	//virtual void setNextCommand();
	void getBuildOrderReady();
	int& getNumberOfUnits(BWAPI::UnitType unit);
	std::map<BWAPI::UnitType, int> getUnitTypesMap();
	BuildingStrategyManager getBuildingStrategyManager();
	UnitStrategyManager getUnitStrategyManager();

};