#pragma once
#include "map"
#include "BWAPI.h"
class CombatStrategyManager
{
public:
	CombatStrategyManager();

	int evaluate();
	void performMiniMax();
};

