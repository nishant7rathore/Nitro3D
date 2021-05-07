#pragma once
#include "map"
#include "BWAPI.h"

class UnitStrategyManager
{
public:
	UnitStrategyManager();

	std::map<BWAPI::UnitType,int> m_unitBuildOrder;
	std::map<BWAPI::UnitType, int> m_deletedUnitsCount;
	std::map<BWAPI::UnitType, int> m_killedUnitsCount;

	//BWAPI::TilePosition getUnitLocation(BWAPI::UnitType unit);
	int getNumberOfUnits(BWAPI::UnitType unit);
	void UnitStrategyManager::trainCombatUnits(std::map<BWAPI::UnitType, int>& completedUnits);
};

