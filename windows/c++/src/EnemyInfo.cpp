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

void EnemyInfo::setRace(std::string val)
{
	m_race = val;
}

BWAPI::Position EnemyInfo::getEnemyBaseLocation()
{
	return m_enemyBaseLocation;
}

BWAPI::Position EnemyInfo::getEnemyStartLocation()
{
	return m_enemyStartLocation;
}

void EnemyInfo::setEnemyBaseLocation(BWAPI::Position val)
{
	m_enemyBaseLocation = val;
}

void EnemyInfo::setEnemyStartLocation(BWAPI::Position val)
{
	m_enemyStartLocation = val;
}

int EnemyInfo::getTotalUnits()
{
	return m_totalUnits;
}

void EnemyInfo::updateTotalUnits(int val)
{
	m_totalUnits += val;
}

std::vector<int> EnemyInfo::getVisited()
{
	return visited;
}

void EnemyInfo::updateVisited(int val)
{
	visited.push_back(val);
}

std::map<BWAPI::UnitType, int> EnemyInfo::getUnitCountMap()
{
	return m_unitCountMap;
}

void EnemyInfo::updateUnitCountMap(BWAPI::UnitType unitType)
{ 
	// check if unitType exists in the map or not
	std::map<BWAPI::UnitType, int>::iterator it = m_unitCountMap.find(unitType);

	// key already present on the map
	if (it != m_unitCountMap.end()) 
	{
		it->second++;    // increment map's value for key
	}
	// key not found
	else 
	{
		m_unitCountMap.insert(std::make_pair(unitType, 1));
	}
}

std::map<BWAPI::UnitType, int> EnemyInfo::getBuildingCountMap()
{
	return m_buildingCountMap;
}

void EnemyInfo::updateBuildingCountMap(BWAPI::UnitType unitType)
{
	// check if unitType exists in the map or not
	std::map<BWAPI::UnitType, int>::iterator it = m_buildingCountMap.find(unitType);

	// key already present on the map
	if (it != m_buildingCountMap.end())
	{
		it->second++;    // increment map's value for key
	}
	// key not found
	else
	{
		m_buildingCountMap.insert(std::make_pair(unitType, 1));
	}
}