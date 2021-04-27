#include "EnemyInfo.h"

EnemyInfo::EnemyInfo()
{
	this->m_unitCount.clear();
}

std::string EnemyInfo::getRace()
{
	return m_race;
}

int EnemyInfo::getTotalUnits()
{
	return m_totalUnits;
}

void EnemyInfo::updateTotalUnits(int val)
{
	m_totalUnits = val;
}

void EnemyInfo::updateUnitCountMap(std::map<BWAPI::UnitType, int>)
{
	std::cout << "do something" << std::endl;
}
