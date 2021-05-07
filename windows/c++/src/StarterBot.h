#pragma once

#include "MapTools.h"

#include <BWAPI.h>
#include <thread>
#include <mutex>
#include <vector>
#include "StrategyManager.h"
#include "EnemyInfo.h"


class StarterBot
{
    MapTools m_mapTools;
	ResourceManager m_resourceManager;
	StrategyManager m_strategyManager;
	EnemyInfo m_enemyInfo;
	std::vector<int> m_visited;
	BWAPI::Unit myScout;
	BWAPI::Unit possibleEnemy = NULL;
	BWAPI::Position enemyPos;
	bool scoutingComplete = false;
	bool enemyFound = false;

	std::thread t1;
	int resourceCount = 100;

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
	void doScouting();
    void sendIdleWorkersToMinerals();
	void storeEnemyInfo();
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
