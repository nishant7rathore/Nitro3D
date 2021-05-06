#pragma once
#include "map"
#include "BWAPI.h"
#include "vector"

class EnemyInfo
{
	BWAPI::TilePosition m_enemyStartLocation;
	BWAPI::TilePosition m_enemyBaseLocation;
	std::string m_race;
	int m_totalUnits;
	std::map<BWAPI::UnitType, int> m_unitCountMap;
	std::map<BWAPI::UnitType, int> m_buildingCountMap;

public:
	EnemyInfo();

	bool expansion;
	std::string getRace();
	BWAPI::TilePosition getEnemyBaseLocation();
	BWAPI::TilePosition getEnemyStartLocation();
	void setEnemyBaseLocation(BWAPI::TilePosition val);
	void setEnemyStartLocation(BWAPI::TilePosition val);
	int getTotalUnits();
	void updateTotalUnits(int);
	std::map<BWAPI::UnitType, int> getUnitCountMap();
	void updateUnitCountMap(BWAPI::UnitType, int);
	std::map<BWAPI::UnitType, int> getBuildingCountMap();
	void updateBuildingCountMap(BWAPI::UnitType, int);
};
