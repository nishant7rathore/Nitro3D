#pragma once
#include "map"
#include "BWAPI.h"
class CombatStrategyManager
{
public:

	CombatStrategyManager();

	int evaluate(BWAPI::Unitset myUnits, BWAPI::Unitset enemyUnits);
	void performMiniMax(BWAPI::Unitset myUnits, BWAPI::Unitset enemyUnits, int currDepth, int maxDepth);
	bool isThisTerminalState(BWAPI::Unitset myUnits);
};