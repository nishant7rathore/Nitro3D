#include "EnemyInfo.h"

EnemyInfo::EnemyInfo()
{
	this->m_unitCountMap.clear();
	this->m_buildingCountMap.clear();
}

std::string EnemyInfo::getRace()
{
	return m_race;
}

BWAPI::TilePosition EnemyInfo::getEnemyBaseLocation()
{
	return m_enemyBaseLocation;
}

BWAPI::TilePosition EnemyInfo::getEnemyStartLocation()
{
	return m_enemyStartLocation;
}

void EnemyInfo::setEnemyBaseLocation(BWAPI::TilePosition val)
{
	m_enemyBaseLocation = val;
}

void EnemyInfo::setEnemyStartLocation(BWAPI::TilePosition val)
{
	m_enemyStartLocation = val;
}

int EnemyInfo::getTotalUnits()
{
	return m_totalUnits;
}

void EnemyInfo::updateTotalUnits(int val)
{
	m_totalUnits = val;
}

std::map<BWAPI::UnitType, int> EnemyInfo::getUnitCountMap()
{
	return m_unitCountMap;
}

void EnemyInfo::updateUnitCountMap(BWAPI::UnitType, int)
{
	std::cout << "do something with unit map" << std::endl;
}

std::map<BWAPI::UnitType, int> EnemyInfo::getBuildingCountMap()
{
	return m_buildingCountMap;
}

void EnemyInfo::updateBuildingCountMap(BWAPI::UnitType, int)
{
	std::cout << "do something with building map" << std::endl;
}