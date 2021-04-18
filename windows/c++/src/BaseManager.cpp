#include "BaseManager.h"


Base::Base(BWAPI::Unit base)
{
	m_buildings.clear();
	m_workers.clear();
	m_base = base;
}

BaseManager::BaseManager()
{
	m_basesMap.clear();
}

Base& BaseManager::getBase(int base)
{
	return m_basesMap[base];
}

std::map<int, Base> BaseManager::getBasesMap()
{
	return m_basesMap;
}

void BaseManager::addOrUpdateBase(BWAPI::Unit base, bool isDefault)
{
	if (isDefault)
	{
		m_basesMap[0] = Base(base);
		m_basesMap[0].m_buildings.push_back(base->getID());
	}
	else
	{
		m_basesMap[m_basesMap.size()] = Base(base);
		m_basesMap[m_basesMap.size()].m_buildings.push_back(base->getID());
	}

}


void BaseManager::checkForInvalidMemory()
{
	std::vector<size_t> invalidIndices;

	for (size_t i = 0; i < m_basesMap.size(); i++)
	{
		if (m_basesMap[i].m_buildings.size() == 0 && m_basesMap[i].m_workers.size() == 0)
		{
			invalidIndices.push_back(i);
		}
	}

	for (size_t i = 0; i < invalidIndices.size(); i++)
	{
		m_basesMap.erase(invalidIndices[i]);
	}
}

void BaseManager::addUnitToBase(BWAPI::Unit unit, int base)
{

	if (unit->getType().isResourceContainer() || base < 0)
	{
		return;
	}

	checkForInvalidMemory();

	if (unit->getType().isBuilding())
	{
		m_basesMap[base].m_buildings.push_back(unit->getID());
		//std::cout << "Adding: " << unit->getType();
	}
	else
	{
		m_basesMap[base].m_workers.push_back(unit->getID());
		//std::cout << "Adding Unit: " << unit->getType();
	}
	
	if (base == 2)
	{
		m_basesMap.erase(2);
	}

}

void BaseManager::addUnitToBase(BWAPI::Unit newUnit, BWAPI::Unit buildUnit)
{
	int baseIndex = getBaseofUnit(buildUnit);

	if (!buildUnit) baseIndex = 0;


	addUnitToBase(newUnit, baseIndex);
}

void BaseManager::removeUnitFromBase(BWAPI::Unit destroyedUnit)
{
	std::map<int, Base>::iterator it;
	std::vector<int>::iterator workersIterator;
	std::vector<int>::iterator buildingsIterator;

	int baseIndex = -1;
	int unitIDIndex = -1;
	bool isWorker = false;
	int count = 0;

	if (!destroyedUnit->getType().isBuilding())
	{
		for (it = m_basesMap.begin(); it != m_basesMap.end(); it++)
		{
			count = 0;

			for (workersIterator = it->second.m_workers.begin(); workersIterator != it->second.m_workers.end(); workersIterator++)
			{
				if ((*workersIterator) == destroyedUnit->getID())
				{
					unitIDIndex = count;
					baseIndex = it->first;
					isWorker = true;
					break;
				}
				count++;
			}
		}
	}
	else
	{
		for (it = m_basesMap.begin(); it != m_basesMap.end(); it++)
		{
			count = 0;
			for (buildingsIterator = it->second.m_buildings.begin(); buildingsIterator != it->second.m_buildings.end(); buildingsIterator++)
			{
				if ((*buildingsIterator) == destroyedUnit->getID())
				{
					unitIDIndex = count;
					baseIndex = it->first;
					isWorker = false;
					break;
				}
			}
		}
	}
	if (baseIndex >=0 && unitIDIndex >= 0)
	{
		if (isWorker)
		{
			m_basesMap[baseIndex].m_workers.erase(m_basesMap[baseIndex].m_workers.begin() + unitIDIndex);
		}
		else
		{
			m_basesMap[baseIndex].m_buildings.erase(m_basesMap[baseIndex].m_buildings.begin() + unitIDIndex);
		}
	}
}

BWAPI::Unit BaseManager::getWorkerFromBase(int base)
{
	if (base < 0) return nullptr;
	if (base > 1)  base = 1;

	std::vector<int>::iterator workersIterator;

	for (workersIterator = m_basesMap[base].m_workers.begin(); workersIterator != m_basesMap[base].m_workers.end(); workersIterator++)
	{
		BWAPI::Unit worker = BWAPI::Broodwar->getUnit((*workersIterator));
		if (worker && worker->exists() && worker->isCompleted() && worker->getType() == BWAPI::UnitTypes::Protoss_Probe)
		{
			return worker;
		}
	}
	return nullptr;
}

int BaseManager::getBaseofUnit(BWAPI::Unit unit)
{
	std::map<int, Base>::iterator it;
	std::vector<int>::iterator workersIterator;
	std::vector<int>::iterator buildingsIterator;

	int baseIndex = -1;
	bool isWorker = false;

	if (unit && !unit->getType().isBuilding())
	{
		for (it = m_basesMap.begin(); it != m_basesMap.end(); it++)
		{
			for (workersIterator = it->second.m_workers.begin(); workersIterator != it->second.m_workers.end(); workersIterator++)
			{
				if ((*workersIterator) == unit->getID())
				{
					isWorker = true;
					baseIndex = it->first;
					break;
				}
			}
		}
	}
	else if (unit)
	{
		for (it = m_basesMap.begin(); it != m_basesMap.end(); it++)
		{
			for (buildingsIterator = it->second.m_buildings.begin(); buildingsIterator != it->second.m_buildings.end(); buildingsIterator++)
			{
				if ((*buildingsIterator) == unit->getID())
				{
					isWorker = false;
					baseIndex = it->first;
					break;
				}
			}
		}
	}
	return baseIndex;
}

const size_t BaseManager::getBuildingsCount(int base, BWAPI::UnitType unitType)
{
	const size_t count = m_basesMap[base].m_buildings.size();

	int sum = 0;

	for (size_t i=0; i<count; i++)
	{
		if (BWAPI::Broodwar->getUnit(m_basesMap[base].m_buildings[i])->getType() == unitType)
		{
			sum++;
		}
	}

	return sum;
}

const size_t BaseManager::getNonBuildingsCount(int base, BWAPI::UnitType unitType)
{
	const size_t count = m_basesMap[base].m_workers.size();

	int sum = 0;

	for (size_t i = 0; i < count; i++)
	{
		if (BWAPI::Broodwar->getUnit(m_basesMap[base].m_workers[i])->getType() == unitType)
		{
			sum++;
		}
	}

	return sum;
}

BWAPI::Unit BaseManager::getUnitOfTypeFromBase(int base, BWAPI::UnitType unitType)
{
	if (unitType.isBuilding())
	{
		const size_t count = m_basesMap[base].m_buildings.size();

		for (size_t i = 0; i < count; i++)
		{
			if (BWAPI::Broodwar->getUnit(m_basesMap[base].m_buildings[i])->getType() == unitType)
			{
				return BWAPI::Broodwar->getUnit(m_basesMap[base].m_buildings[i]);
			}
		}
	}
	else
	{
		const size_t count = m_basesMap[base].m_workers.size();

		for (size_t i = 0; i < count; i++)
		{
			if (BWAPI::Broodwar->getUnit(m_basesMap[base].m_workers[i])->getType() == unitType)
			{
				return BWAPI::Broodwar->getUnit(m_basesMap[base].m_workers[i]);
			}
		}
	}

	return nullptr;
}