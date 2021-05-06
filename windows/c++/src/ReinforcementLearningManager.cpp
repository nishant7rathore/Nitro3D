#include "Tools.h"
#include "MapTools.h"
#include <iostream>
#include <fstream>
#include "ReinforcementLearningManager.h"
#include <chrono>
#include <iomanip>

ResultsSaver::ResultsSaver(StrategyManager& strategyManager) :m_stratManager(strategyManager)
{
	const auto clock = std::chrono::system_clock::now();
	m_fileName = "Results_"+std::to_string(std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count())+".txt";
	initializeBandits();
}

void ResultsSaver::saveData(bool isWin)
{
	populateUnitKilledCount();
	updateBanditData();
	//game statistics
	m_gameCount++;
	isWin ? m_gamesWon++ : m_gamesLost++;

	std::ofstream writer;
	if (isFileExists(m_fileName))
	{
		writer.open(m_fileName, std::ios_base::app);
	}
	else
	{
		writer.open(m_fileName);
	}

	writer << m_gameCount << "  " << m_gamesWon << " " << m_gamesLost << " " << BWAPI::Broodwar->enemy()->getRace()<<std::endl;
	writer.close();
}

bool ResultsSaver::isFileExists(std::string fileName)
{
	std::ifstream infile(fileName);
	bool ret = infile.good();
	infile.close();
	return ret;
}

void ResultsSaver::populateUnitKilledCount()
{
	for (auto& unit: BWAPI::Broodwar->self()->getUnits())
	{
		if (!unit->isCompleted() || !unit->exists())
		{
			continue;
		}

		if (unit->getType() != BWAPI::UnitTypes::Protoss_Zealot || unit->getType() != BWAPI::UnitTypes::Protoss_Dragoon || 
			unit->getType() != BWAPI::UnitTypes::Protoss_Dark_Templar || unit->getType() != BWAPI::UnitTypes::Protoss_Corsair)
		{
			continue;
		}

		m_stratManager.getUnitStrategyManager().m_killedUnitsCount[unit->getType()] += unit->getKillCount();
		m_stratManager.getUnitStrategyManager().m_deletedUnitsCount[unit->getType()] += 1;
	}
}

void ResultsSaver::initializeBandits()
{
	m_bandits.clear();
	std::string fileName = "BanditsData.txt";

	if (isFileExists(fileName))
	{
		std::ifstream fin(fileName);

		int ID, numAction = 0;
		double estimatedValue = 0.0;

		while (fin.good())
		{
			fin >> ID >> numAction >> estimatedValue;
			m_bandits.push_back(Bandit(ID,numAction,estimatedValue));
		}
		fin.close();
	}

}

void ResultsSaver::updateBanditData()
{
	std::ofstream writer;
	writer.open("BanditsData.txt");

	if (m_bandits.size() == 0)
	{
		m_bandits.push_back(Bandit(BWAPI::UnitTypes::Protoss_Zealot, 0, 0.0));
		m_bandits.push_back(Bandit(BWAPI::UnitTypes::Protoss_Dragoon, 0, 0.0));
		m_bandits.push_back(Bandit(BWAPI::UnitTypes::Protoss_Dark_Templar, 0, 0.0));
		m_bandits.push_back(Bandit(BWAPI::UnitTypes::Protoss_Corsair, 0, 0.0));
	}

	for (Bandit bandit:m_bandits)
	{
		double newValEstimate = getNewEstimatedValue(bandit);
		bandit.getValueEstimate() = newValEstimate;
		writer << bandit.getID() << "  ";
		writer << bandit.getNumOfActions() << "  ";;
		writer << bandit.getValueEstimate() << "  " <<std::endl;
	}
		
}

double ResultsSaver::getNewEstimatedValue(Bandit bandit)
{
	double epsilon = 0.15; //hardcoding this value for now
	double randomEpsilonVal = (rand()%100)/100;
	double reward = 0.0; 
	double valueEstimate = bandit.getValueEstimate();

	if (randomEpsilonVal < epsilon)
	{
		reward = m_stratManager.getUnitStrategyManager().m_killedUnitsCount[bandit.getID()];
	}
	else
	{
		reward = bandit.getValueEstimate();
	}

	int& numActions = bandit.getNumOfActions();
	numActions += 1;
	valueEstimate = valueEstimate + (1.0 / numActions) * (reward - valueEstimate);

	return valueEstimate;

}

int Bandit::getID()
{
	return m_id;
}

int& Bandit::getNumOfActions()
{
	return m_numOfActions;
}

double& Bandit::getValueEstimate()
{
	return m_valueEstimate;
}
