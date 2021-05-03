#include "CombatStrategyManager.h"

CombatStrategyManager::CombatStrategyManager()
{
}

int CombatStrategyManager::evaluate(BWAPI::Unitset myUnits, BWAPI::Unitset enemyUnits)
{
	return 0;
}

int numMyUnits = 5;
int numTheirUnits = 5;

void CombatStrategyManager::performMiniMax(BWAPI::Unitset myUnits, BWAPI::Unitset enemyUnits, int currDepth, int maxDepth, bool isMax)
{
	if (isThisTerminalState(myUnits) || currDepth > maxDepth)
	{
		
	}


	BWAPI::Unitset stateUnitSet;

	for (auto& unit: myUnits)
	{
		stateUnitSet.emplace(unit);
	}
}

bool CombatStrategyManager::isThisTerminalState(BWAPI::Unitset myUnits)
{
	return false;
}
