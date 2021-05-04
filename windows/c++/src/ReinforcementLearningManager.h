
#include "StarterBot.h"
#pragma once

class ResultsSaver
{
private:
	StrategyManager& m_stratManager;
	int m_gameCount;
	int m_gamesWon;
	int m_gamesLost;
	std::string m_fileName = "Results.txt";

public:
	ResultsSaver() :m_stratManager(StrategyManager()) {};
	ResultsSaver(StrategyManager& strategyManager);
	void saveData(bool isWin);
	bool isFileExists();
	void populateUnitKilledCount();
};