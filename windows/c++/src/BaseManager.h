#pragma once
#include <map>
#include "BWAPI.h"

struct Base
{
	BWAPI::Unit m_base;
	std::vector<int> m_workers;
	std::vector<int> m_buildings;
	int m_workersWanted=0;

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
		void addOrUpdateBase(BWAPI::Unit base, int baseIndex);
		void checkForInvalidMemory();
		void addUnitToBase(BWAPI::Unit unit, int base);
		void addUnitToBase(BWAPI::Unit newUnit, BWAPI::Unit buildUnit);
		void removeUnitFromBase(BWAPI::Unit destroyedUnit);
		BWAPI::Unit getWorkerFromBase(int base);
		int getBaseofUnit(BWAPI::Unit unit);
		const size_t getBuildingsCount(int base, BWAPI::UnitType unitType, bool isCompleted);
		const size_t getNonBuildingsCount(int base, BWAPI::UnitType unitType);
		BWAPI::Unit getUnitOfTypeFromBase(int base, BWAPI::UnitType unitType);
};

