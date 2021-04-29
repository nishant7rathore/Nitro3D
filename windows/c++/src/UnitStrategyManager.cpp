#include "UnitStrategyManager.h"
#include "Tools.h"

UnitStrategyManager::UnitStrategyManager()
{
	m_unitBuildOrder.clear();
	m_unitBuildOrder.emplace(BWAPI::UnitTypes::Protoss_Zealot,40);
	m_unitBuildOrder.emplace(BWAPI::UnitTypes::Protoss_Dragoon, 40);
    m_unitBuildOrder.emplace(BWAPI::UnitTypes::Protoss_Dark_Templar, 20);
    m_unitBuildOrder.emplace(BWAPI::UnitTypes::Protoss_Corsair, 10);
    m_unitBuildOrder.emplace(BWAPI::UnitTypes::Protoss_Observer, 5);
}

int UnitStrategyManager::getNumberOfUnits(BWAPI::UnitType unit)
{
	return m_unitBuildOrder[unit];
}

void UnitStrategyManager::trainCombatUnits(std::map<BWAPI::UnitType,int>& completedUnits)
{

    std::map<BWAPI::UnitType, int>::iterator it;
    BWAPI::Unit builder = nullptr;
    for (it= m_unitBuildOrder.begin(); it != m_unitBuildOrder.end(); it++)
    {
        builder = Tools::GetTrainerUnitNotFullOfType(it->first);

        if (builder && BWAPI::Broodwar->canMake(it->first))
        {
            if (completedUnits[it->first] <= it->second)
            {
                bool isSuccess = builder->train(it->first);
                if (isSuccess) std::cout << "Training " << it->first;
            }
            
        }
    }
}