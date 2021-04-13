#pragma once

#include "MapTools.h"

#include <BWAPI.h>
#include <thread>
#include <mutex>
#include "StrategyManager.h"


class StarterBot
{
    MapTools m_mapTools;
	ResourceManager m_resourceManager;
	StrategyManager m_strategyManager;

	std::thread t1;
	int resourceCount = 100;
	int m_workersWanted = 15;

	BWAPI::Unitset zealots;
	BWAPI::Unitset defenders;
	BWAPI::Unitset workers;
	BWAPI::Position unitPosition = BWAPI::Positions::Invalid;
	BWAPI::UnitCommandType zealotsLastCommandType = BWAPI::UnitCommandTypes::None;
	int lastEnemeyAttackingID = -1;

public:

	std::thread& getThreadHandle();

    StarterBot(ResourceManager& rm);

	// helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
	void doUpgrades();
	void findAdditionalBases();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
    void drawDebugInformation();
	void buildArmy();
	void buildBuildings ();

    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitDiscover(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);
};
