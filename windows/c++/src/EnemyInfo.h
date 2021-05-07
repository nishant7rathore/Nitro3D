#pragma once
#include "map"
#include "BWAPI.h"
#include "vector"

class EnemyInfo
{
	BWAPI::Position m_enemyStartLocation;
	BWAPI::Position m_enemyBaseLocation;
	std::string m_race;
	int m_totalUnits;
	std::vector<int> visited;
	std::map<BWAPI::UnitType, int> m_unitCountMap;
	std::map<BWAPI::UnitType, int> m_buildingCountMap;

public:
	EnemyInfo();
	std::string getRace();
	void setRace(std::string val);
	BWAPI::Position getEnemyBaseLocation();
	BWAPI::Position getEnemyStartLocation();
	void setEnemyBaseLocation(BWAPI::Position val);
	void setEnemyStartLocation(BWAPI::Position val);
	int getTotalUnits();
	void updateTotalUnits(int val);
	std::vector<int> getVisited();
	void updateVisited(int val);
	std::map<BWAPI::UnitType, int> getUnitCountMap();
	void updateUnitCountMap(BWAPI::UnitType unitType);
	std::map<BWAPI::UnitType, int> getBuildingCountMap();
	void updateBuildingCountMap(BWAPI::UnitType unitType);
};
