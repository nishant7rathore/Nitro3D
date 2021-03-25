#pragma once
#include <BWAPI.h>
class StrategyManager
{
	BWAPI::UnitCommand nextCommand;
public:
	BWAPI::UnitCommand getNextCommand();
	void setNextCommand();
	void readGameState();
};

