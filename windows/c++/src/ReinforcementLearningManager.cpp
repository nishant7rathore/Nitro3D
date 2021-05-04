#include "Tools.h"
#include "MapTools.h"
#include <iostream>
#include <fstream>
#include "ReinforcementLearningManager.h"
#include <chrono>

ResultsSaver::ResultsSaver(StrategyManager& strategyManager) :m_stratManager(strategyManager)
{
	const auto clock = std::chrono::system_clock::now();
	m_fileName = "Results_"+std::to_string(std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count())+".txt";
}


void ResultsSaver::saveData(bool isWin)
{
	populateUnitKilledCount();
	std::ofstream writer;

	if (isFileExists())
	{
		writer.open(m_fileName, std::ios_base::app);
	}

	else
	{
		writer.open(m_fileName);
	}

	writer << "Writing to the file " << isWin << std::endl;
}

bool ResultsSaver::isFileExists()
{
	std::ifstream infile(m_fileName);
	return infile.good();
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

		m_stratManager.getUnitStrategyManager().m_deletedUnitsCount[unit->getType()] += unit->getKillCount();
	}
}


