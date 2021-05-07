#pragma once
#include "map"
#include "BWAPI.h"
class CombatStrategyManager
{
	int m_numMyUnits = 5;
	int m_numTheirUnits = 5;

public:

	CombatStrategyManager();

	int evaluate(BWAPI::Unitset myUnits, BWAPI::Unitset enemyUnits);
	void performMiniMax(BWAPI::Unitset myUnits, BWAPI::Unitset enemyUnits, int currDepth, int maxDepth, bool isMax);
	bool isThisTerminalState(BWAPI::Unitset myUnits);
};