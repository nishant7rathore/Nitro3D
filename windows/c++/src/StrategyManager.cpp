#include "StrategyManager.h"

StrategyManager::StrategyManager()
{
	m_unitTypesToTotal.clear();
	m_unitTypesToTotal.emplace(BWAPI::UnitTypes::Protoss_Nexus, 1);
	m_unitTypesToTotal.emplace(BWAPI::UnitTypes::Protoss_Probe, 4);

}

void StrategyManager::getBuildOrderReady()
{
	//check if nothing else is built 
	if (m_unitTypesToTotal.size() == 1)
	{

	}
}

int& StrategyManager::getNumberOfUnits(BWAPI::UnitType unit)
{
	return m_unitTypesToTotal[unit];
}

int& StrategyManager::getNumberOfCompletedUnits(BWAPI::UnitType unit)
{
	return m_unitTypesToTotalCompleted[unit];
}

std::map<BWAPI::UnitType, int> StrategyManager::getUnitTypesCompletedMap()
{
	return m_unitTypesToTotalCompleted;
}


std::map<BWAPI::UnitType, int> StrategyManager::getUnitTypesMap()
{
	return m_unitTypesToTotal;
}

BuildingStrategyManager& StrategyManager::getBuildingStrategyManager()
{
	return m_buildingStrategyManager;
}

UnitStrategyManager StrategyManager::getUnitStrategyManager()
{
	return m_unitStrategyManager;
}