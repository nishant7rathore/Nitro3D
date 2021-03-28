#include "UnitStrategyManager.h"

UnitStrategyManager::UnitStrategyManager()
{
	m_unitBuildOrder.clear();
	m_unitBuildOrder.emplace(BWAPI::UnitTypes::Protoss_Zealot,5);
	m_unitBuildOrder.emplace(BWAPI::UnitTypes::Protoss_Dragoon, 5);
}

int UnitStrategyManager::getNumberOfUnits(BWAPI::UnitType unit)
{
	return m_unitBuildOrder[unit];
}

