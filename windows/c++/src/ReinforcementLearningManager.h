
#include "StarterBot.h"
#pragma once


class Bandit
{
private:

	int m_id;
	int m_numOfActions;
	double m_valueEstimate;

public:

	Bandit() {};
	Bandit(int ID, int numOfActions, double valueEstimate) :m_id(ID), m_numOfActions(numOfActions), m_valueEstimate(valueEstimate) {};
	int getID();
	int& getNumOfActions();
	double& getValueEstimate();

};

class ResultsSaver
{
private:
	StrategyManager& m_stratManager;
	int m_gameCount=0;
	int m_gamesWon=0;
	int m_gamesLost=0;
	std::string m_fileName = "Results.txt";
	std::vector<Bandit> m_bandits;
public:
	ResultsSaver() :m_stratManager(StrategyManager()) {};
	ResultsSaver(StrategyManager& strategyManager);
	void initializeBandits();
	void saveData(bool isWin);
	bool isFileExists(std::string fileName);
	void populateUnitKilledCount();
	void updateBanditData();
	double getNewEstimatedValue(Bandit bandit);
};

