#pragma once
#include <BWAPI.h>
#include <map>
#include "BuildingStrategyManager.h"
#include "UnitStrategyManager.h"
#include "BaseManager.h"


class StrategyManager
{
	BWAPI::UnitCommand nextCommand;

	std::map<BWAPI::UnitType, int> m_unitTypesToTotal;
	std::map<BWAPI::UnitType, int> m_unitTypesToTotalCompleted;

	BuildingStrategyManager m_buildingStrategyManager;
	UnitStrategyManager m_unitStrategyManager;
	BaseManager m_baseManager;

public:

	StrategyManager();
	StrategyManager(Grid<int>& walkable, Grid<int>& buildable);

	//virtual BWAPI::UnitCommand getNextCommand();
	//virtual void setNextCommand();
	void getBuildOrderReady();
	int& getNumberOfUnits(BWAPI::UnitType unit);
	int& getNumberOfCompletedUnits(BWAPI::UnitType unit);
	std::map<BWAPI::UnitType, int> getUnitTypesCompletedMap();
	std::map<BWAPI::UnitType, int> getUnitTypesMap();
	BuildingStrategyManager& getBuildingStrategyManager();
	UnitStrategyManager& getUnitStrategyManager();
	BaseManager& getBaseManager();

};