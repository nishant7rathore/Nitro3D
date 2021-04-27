#pragma once
#include "map"
#include "BWAPI.h"
#include "vector"

class EnemyInfo
{
	BWAPI::TilePosition m_enemyLocation;
	std::string m_race;
	int m_totalUnits;
	std::map<BWAPI::UnitType, int> m_unitCount;

public:
	EnemyInfo();

	bool expansion;
	std::string getRace();
	int getTotalUnits();
	void updateTotalUnits(int);
	void updateUnitCountMap(std::map <BWAPI::UnitType, int>);
};
