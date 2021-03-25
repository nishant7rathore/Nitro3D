#pragma once

#include "MapTools.h"

#include <BWAPI.h>
#include <thread>
#include <mutex>


class ResourceManager
{

public:

	Grid<BWAPI::Unit> m_mineralResource;      // mineral resource handle
	Grid<BWAPI::Unit> m_refineryResource;      // refinery resource handle

	ResourceManager();
	void calculateDistances();
	BWAPI::Unit getMineralResource(int x, int y);
	void setMineralResource(int x, int y, BWAPI::Unit unit);
	BWAPI::Unit getRefineryResource(int x, int y);
	void setRefineryResource(int x, int y, BWAPI::Unit unit);
};

class StarterBot
{
    MapTools m_mapTools;
	ResourceManager m_resourceManager;
	std::thread t1;
	int count = 7;

public:


    StarterBot(ResourceManager& rm);

	// helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
    void drawDebugInformation();

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
