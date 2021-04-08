#include "Tools.h"
#include "MapTools.h"
#include "StarterBot.h"
#include "../visualstudio/Profiler.hpp"

/**
* Nishant Rathore 
* Rohan Parmar
* 
* We have an extension for this assignment, but after chatting with Dave, I was instructed to submit this. 
* 
*/

bool isAdditionalSupplyNeeded = false;

std::thread& StarterBot::getThreadHandle()
{
    return std::ref(t1);
}

StarterBot::StarterBot(ResourceManager& rm)
{
    m_resourceManager = rm;
    m_strategyManager = StrategyManager();

}

bool isZerglingRush = false;

// Called when the bot starts!
void StarterBot::onStart()
{
    resourceCount = 100;
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    //TODO: need to ask Dave about this flag
    //BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);

    // Call MapTools OnStart
    //t1 = std::thread(&MapTools::onStart,std::ref(m_mapTools),std::ref(m_resourceManager));
    m_mapTools.onStart(m_resourceManager);
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner) 
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    PROFILE_FUNCTION();

    // Update our MapTools information
    m_mapTools.onFrame();

    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    buildBuildings();

    buildArmy();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}


std::mutex m;

void ResourceManager::calculateDistances()
{
    std::lock_guard<std::mutex> guard(m);

    int height = BWAPI::Broodwar->mapHeight();
    int width = BWAPI::Broodwar->mapWidth();

    for (int i=0;i<height;i++) 
    {
        for (int j = 0; j<width; j++)
        {
            BWAPI::Position pos(BWAPI::TilePosition(j, i));
            Resource closestMineral = Tools::GetClosestResourceMineralToUnit(pos);
            Resource closestGeyeser = Tools::GetClosestGeyserToUnit(pos);
            setMineralResource(j, i, closestMineral);
            setRefineryResource(j, i, closestGeyeser);
        }
    }

    isJobComplete = true;
}

bool isRefBuilt = false;

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{

    //if (!t1.joinable()) t1.detach();


    if (isZerglingRush && workers.getClosestUnit(BWAPI::Filter::IsAttacking))
    {
        workers.attack(workers.getClosestUnit(BWAPI::Filter::IsAttacking));
        return;
    }

    isZerglingRush = false;

    PROFILE_FUNCTION();

    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    BWAPI::TilePosition refineryPosition(0,0);



    for (auto& unit : myUnits)
    {

        // Get the closest mineral to this worker unit

        auto u = m_resourceManager.getMineralResource(unit->getTilePosition().x, unit->getTilePosition().y);

        //std::string s = std::to_string(unit->getTilePosition().x) + " " + std::to_string(unit->getTilePosition().y);

        //std::string st = std::to_string(u->getTilePosition().x) + " " + std::to_string(u->getTilePosition().y);

        BWAPI::Position pos(BWAPI::TilePosition(unit->getTilePosition().x, unit->getTilePosition().y));
        BWAPI::Unit closestMineral = BWAPI::Broodwar->getUnit(u.m_id);


       // int closMinID = closestMineral->getID();

        //std::string t = std::to_string(closestMineral->getTilePosition().x) + " " + std::to_string(closestMineral->getTilePosition().y);

        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle() && !unit->isMoving() && !unit->isAttacking())
        {

            int mineralsLeft = BWAPI::Broodwar->self()->minerals();
            //
            int mineralPrice = BWAPI::Broodwar->self()->getRace().getRefinery().mineralPrice();

            int& numAssimiltorCompletedUnits = m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Assimilator);

            //int numAssimilitorUnits = BWAPI::Broodwar->getUnitsOnTile(m_resourceManager.getRefineryResource(unit->getTilePosition().x, unit->getTilePosition().y).m_x, m_resourceManager.getRefineryResource(unit->getTilePosition().x, unit->getTilePosition().y).m_y).size();

            if (closestMineral) { unit->rightClick(closestMineral); }
          
        }
    }
}

// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers()
{

    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersWanted = 12;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    // get the unit pointer to my depot
    const BWAPI::Unit myDepot = Tools::GetDepot();
    if (workersOwned <= workersWanted)
    {
        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
 
}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
        
    PROFILE_FUNCTION();

    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply > 8) 
    {
        return;
    }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    bool startedBuilding = Tools::BuildBuilding(supplyProviderType, m_strategyManager.getBuildingStrategyManager());
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }

}


// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "RathoreParmar\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

void StarterBot::buildBuildings()
{
    if (m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Pylon) < 1) return;

    std::map<BWAPI::UnitType, int> map = m_strategyManager.getBuildingStrategyManager().getBuildingOrderMap();
    std::map<BWAPI::UnitType, int>::iterator it;

    for (it = map.begin(); it != map.end(); it++)
    {

        int mineralsLeft = BWAPI::Broodwar->self()->minerals();

        int mineralPrice = it->first.mineralPrice();

        int gasLeft = BWAPI::Broodwar->self()->gas();

        int gasPrice = it->first.gasPrice();

        int& numUnits = m_strategyManager.getNumberOfUnits(it->first);

        if (BWAPI::Broodwar->self()->isUnitAvailable(it->first) && numUnits < m_strategyManager.getBuildingStrategyManager().getNumberOfBuildings(it->first) && mineralsLeft >= mineralPrice && gasLeft >= gasPrice)
        {
            bool built = Tools::BuildBuilding(it->first, m_strategyManager.getBuildingStrategyManager());

            if (built)
            {
                if (it->first == BWAPI::UnitTypes::Protoss_Assimilator) numUnits++;

                //numUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Gateway, BWAPI::Broodwar->self()->getUnits());
                //std::cout << "Gateway Built " << numUnits << std::endl;
                BWAPI::Broodwar->printf("Started Building %s", it->first.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Gateway]= numUnits;
            }
        }
    }
}

void StarterBot::buildArmy()
{
    BWAPI::Unit builder = Tools::GetTrainerUnitNotFullOfType(BWAPI::UnitTypes::Protoss_Zealot);
    if (builder)
    {
        if (!Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Templar_Archives, BWAPI::Broodwar->self()->getUnits()))
        {
            if (Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot, BWAPI::Broodwar->self()->getUnits()) <= 5)
            {
                builder->train(BWAPI::UnitTypes::Protoss_Zealot);
            }
            if (Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Dragoon, BWAPI::Broodwar->self()->getUnits()) <= 5)
            {
                builder->train(BWAPI::UnitTypes::Protoss_Dragoon);
            }
        }
        else if (Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Dark_Templar, BWAPI::Broodwar->self()->getUnits()) < 5)
        {
            builder->train(BWAPI::UnitTypes::Protoss_Dark_Templar);
        }
        else if (Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Corsair, BWAPI::Broodwar->self()->getUnits()) < 5)
        {
            builder = Tools::GetTrainerUnitNotFullOfType(BWAPI::UnitTypes::Protoss_Corsair);
            if (builder) builder->train(BWAPI::UnitTypes::Protoss_Corsair);
        }

    }

    bool isUnderAttack = false;

    if (zealots.size() == 5)
    {

        for (BWAPI::TilePosition pos : BWAPI::Broodwar->getStartLocations())
        {
            int count = 0;

            for (auto it = zealots.begin(); it != zealots.end() && count < 5; it++)
            {
                count++;
                if (!BWAPI::Broodwar->isExplored(pos))
                {
                    BWAPI::Unit unit = *it;
                    BWAPI::Unit enemeyUnit = BWAPI::Broodwar->getClosestUnit(unit->getPosition(), BWAPI::Filter::IsEnemy);
                    if (enemeyUnit)
                    {
                        zealots.attack(enemeyUnit);
                        isUnderAttack = true;
                        break;
                    }
                   
                }
            }
            if (isUnderAttack)
            {
                break;
            }
            else if (!BWAPI::Broodwar->isExplored(pos))
            {
                zealots.move(BWAPI::Position(pos));
            }

        }

    }
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
    if (unit->getType() == BWAPI::UnitTypes::Protoss_Photon_Cannon)
    {
        BWAPI::Unit worker = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());
        if (worker->build(unit->getType(), BWAPI::TilePosition(unit->getPosition())))
        {
            //BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Photon_Cannon.getName().c_str());
        }
    }
    if (unit && (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot || unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon || unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar))
    {
        if (defenders.size())
        {
            defenders.erase(unit);
        }

        if (zealots.size())
        {
            zealots.erase(unit);
        }

    }

}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{

}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text)
{
    if (text == "map")
    {
        m_mapTools.toggleDraw();
    }
    BWAPI::Broodwar->sendText("%s", text.c_str());
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{
    
    int& numUnits = m_strategyManager.getNumberOfUnits(unit->getType());
    numUnits++;

}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{

    int& numCompletedUnits = m_strategyManager.getNumberOfCompletedUnits(unit->getType());
    numCompletedUnits++;

    if (m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Assimilator) && unit->getType().isWorker())
    {
        unit->rightClick(Tools::GetUnitOfType(BWAPI::UnitTypes::Protoss_Assimilator));
    }

    if (unit->getType().isBuilding())
    {
        if (unit->getType().isRefinery())
        {
            Tools::GetDepot()->train(BWAPI::UnitTypes::Protoss_Probe);
            Tools::GetDepot()->train(BWAPI::UnitTypes::Protoss_Probe);
        }
        //if (unit->getType().buildsWhat().count(BWAPI::UnitTypes::Protoss_Zealot))
        //{
        //    int& numUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Zealot);
        //    int& numDragoonUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Dragoon);
        //    if (numUnits < m_strategyManager.getUnitStrategyManager().getNumberOfUnits(BWAPI::UnitTypes::Protoss_Zealot))
        //    {
        //        //numUnits++;
        //        if (unit->train(BWAPI::UnitTypes::Protoss_Zealot)) m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Zealot] = numUnits;
        //        numUnits = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot,BWAPI::Broodwar->self()->getUnits());
        //    }
        //    if (numDragoonUnits < m_strategyManager.getUnitStrategyManager().getNumberOfUnits(BWAPI::UnitTypes::Protoss_Dragoon))
        //    {
        //        //numDragoonUnits++;
        //        if (unit->train(BWAPI::UnitTypes::Protoss_Dragoon)) m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Dragoon] = numDragoonUnits;
        //        numUnits = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Dragoon, BWAPI::Broodwar->self()->getUnits());
        //    }
        //}
        //else if (unit->getType().isRefinery())
        //{
        //    Tools::GetDepot()->train(BWAPI::UnitTypes::Protoss_Probe);
        //    Tools::GetDepot()->train(BWAPI::UnitTypes::Protoss_Probe);
        //}
    }

    else if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot || unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon || unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar)
    {
        if (defenders.size() < 5) 
        {
            defenders.emplace(unit);
        }
        else if (zealots.size() < 5) 
        {
            zealots.emplace(unit);
        }
        else
        {
            defenders.emplace(unit);
        }
    }
    else
    {
        workers.emplace(unit);
    }
   
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
    //std::cout << unit->getType().getName();
}

void StarterBot::onUnitDiscover(BWAPI::Unit unit)
{
    if (unit->isAttacking() && unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        defenders.attack(unit);
    }
    else if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) && unit->getType() == BWAPI::UnitTypes::Zerg_Zergling && unit->getDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation())) < 1000)
    {
        if(defenders.size() < 2) isZerglingRush = true;
    }


}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}

Resource::Resource(int id, size_t x, size_t y, int resourceAmount, bool isGeyser): 
                                                                        m_id(id),
                                                                        m_x(x),
                                                                        m_y(y),
                                                                        m_resourceAmount(resourceAmount), 
                                                                        m_isGeyser(isGeyser)
{

}

ResourceManager::ResourceManager()
{
    int m_width = BWAPI::Broodwar->mapWidth();
    int m_height = BWAPI::Broodwar->mapHeight();
    m_mineralResource = Grid<Resource>(m_width, m_height, Resource());
    m_refineryResource = Grid<Resource>(m_width, m_height, Resource());
}

Resource ResourceManager::getMineralResource(int x, int y)
{
    return m_mineralResource.get(x, y);
}

void ResourceManager::setMineralResource(int x, int y, Resource unit)
{
    m_mineralResource.set(x, y, unit);
}

Resource ResourceManager::getRefineryResource(int x, int y)
{
    return m_refineryResource.get(x, y);
}

void ResourceManager::setRefineryResource(int x, int y, Resource unit)
{
    m_refineryResource.set(x, y, unit);
}
