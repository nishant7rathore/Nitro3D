#include "StrategyManager.h"

StrategyManager::StrategyManager():m_buildingStrategyManager(BuildingStrategyManager())
{
	m_unitTypesToTotal.clear();

}


StrategyManager::StrategyManager(Grid<int>& walkable, Grid<int>& buildable):m_buildingStrategyManager(walkable, buildable)
{
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

UnitStrategyManager& StrategyManager::getUnitStrategyManager()
{
	return m_unitStrategyManager;
}

BaseManager& StrategyManager::getBaseManager()
{
	return m_baseManager;
}


