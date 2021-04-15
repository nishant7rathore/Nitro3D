#pragma once
#include <map>
#include "BWAPI.h"

struct Base
{
	BWAPI::Unit m_base;
	std::vector<int> m_workers;
	std::vector<int> m_buildings;

	Base() {};
	Base(BWAPI::Unit base);
};

class BaseManager
{
	std::map<int, Base> m_basesMap;

public:
		BaseManager();
		Base& getBase(int base);
		std::map<int, Base> getBasesMap();
		void BaseManager::addOrUpdateBase(BWAPI::Unit base, bool isDefault);
		void addUnitToBase(BWAPI::Unit unit, int base);
		void addUnitToBase(BWAPI::Unit newUnit, BWAPI::Unit buildUnit);
		void removeUnitFromBase(BWAPI::Unit destroyedUnit);
		BWAPI::Unit getWorkerFromBase(int base);
		int BaseManager::getBaseofUnit(BWAPI::Unit unit);
		const size_t getBuildingsCount(int base, BWAPI::UnitType unitType);
		const size_t getNonBuildingsCount(int base, BWAPI::UnitType unitType);
		BWAPI::Unit getUnitOfTypeFromBase(int base, BWAPI::UnitType unitType);
};

