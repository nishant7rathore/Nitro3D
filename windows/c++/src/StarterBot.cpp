#include "Tools.h"
#include "MapTools.h"
#include "StarterBot.h"
#include "../visualstudio/Profiler.hpp"
#include "AStarPathFinding.h"
#include <regex>
#include "ReinforcementLearningManager.h"

/**
* Nishant Rathore 
* Rohan Parmar
* 
*/

std::thread& StarterBot::getThreadHandle()
{
    return std::ref(t1);
}

StarterBot::StarterBot(ResourceManager& rm):m_strategyManager(StrategyManager(m_mapTools.m_walkable, m_mapTools.m_buildable))
{
    m_resourceManager = rm;

}

bool isZerglingRush = false;
BWAPI::Unit myScout;
BWAPI::Unit possibleEnemy = NULL;
BWAPI::Position enemyPos;
bool scoutingComplete = false;
bool enemyFound = false;

// Called when the bot starts!
void StarterBot::onStart()
{
    resourceCount = 100;
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    

    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    myScout = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());


    //TODO: need to ask Dave about this flag
    //BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);

    // Call MapTools OnStart
    //t1 = std::thread(&MapTools::onStart,std::ref(m_mapTools),std::ref(m_resourceManager));
    m_mapTools.onStart(m_resourceManager,m_strategyManager.getBuildingStrategyManager());
    //m_strategyManager.getBuildingStrategyManager().findCannonBuildingLocation(0, m_mapTools.m_walkable, m_mapTools.m_buildable);
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner) 
{
    ResultsSaver rs = ResultsSaver(m_strategyManager);
    rs.saveData(isWinner);
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

    //doScouting();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    buildBuildings();

    buildArmy();

    doUpgrades();

    findAdditionalBases();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();

}

BWAPI::TilePosition baseLocationTilePos = BWAPI::TilePositions::Invalid;
BWAPI::TilePosition nexusTilePos;

void StarterBot::doScouting()
{
    auto& startLocations = BWAPI::Broodwar->getStartLocations();
    BWAPI::Broodwar->drawCircleMap(myScout->getPosition(), 128, BWAPI::Colors::Red, true);
    if (!enemyFound) {
        for (BWAPI::TilePosition tp : startLocations)
        {
            if (!enemyFound)
            {
                if (BWAPI::Broodwar->isExplored(tp)) { continue; }
                if (BWAPI::Broodwar->self()->getStartLocation() == tp) { continue; }
              /*  if (!myScout->isIdle())
                {
                    continue;
                }*/

                BWAPI::Position pos(tp);
                myScout->move(pos);
                if (myScout->getClosestUnit(BWAPI::Filter::IsEnemy, 256))
                {
                    possibleEnemy = myScout->getClosestUnit(BWAPI::Filter::IsEnemy, 256);
                    
                }

                if (possibleEnemy || BWAPI::Broodwar->enemy()->getUnits().size() > 0)
                {
                    enemyPos = pos;
                    enemyFound = true;
                    std::cout << "(" << enemyPos.x << ", " << enemyPos.y << ")" << std::endl;
                    scoutingComplete = true;
                    if (myScout->getPosition() != BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()))
                    {
                        myScout->move(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
                    }
                    std::cout << "FINALLY REACHED" << std::endl;
                    myScout->stop();
                }
            }
        }
    }
}


void StarterBot::findAdditionalBases()
{
    if (m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded()) return;

    int& workerID = m_strategyManager.getBuildingStrategyManager().getWorkerID();
    int& numOfNexus = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Nexus);
    int& numOfCannons = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);

    if (defenders.size() < 5 || zealots.size() < 5 || numOfCannons < 4 || numOfNexus > 1)
    {
        workerID = -1;
        return;
    }
    
    if (workerID != -1 && BWAPI::Broodwar->getUnit(workerID)->exists())
    {
        if (!BWAPI::Broodwar->isExplored(baseLocationTilePos))
        {
            BWAPI::Broodwar->getUnit(workerID)->move(BWAPI::Position(baseLocationTilePos));
        }
        return;
    }


    BWAPI::Unit worker = Tools::GetWorkerExcluding(-1,0, m_strategyManager.getBaseManager());
    if (!worker) return;

    for (auto& baseLocationResource : m_mapTools.m_aStarPathFinding.m_baseLocations)
    {
        BWAPI::TilePosition baseLocation = BWAPI::TilePosition(baseLocationResource.m_x, baseLocationResource.m_y);
        BWAPI::Unit closestUnit = BWAPI::Broodwar->getClosestUnit(BWAPI::Position(baseLocation), BWAPI::Filter::IsResourceDepot, 256);
        if (!closestUnit)
        {
            if (worker->move(BWAPI::Position(baseLocation)))
            {
                workerID = worker->getID();
                baseLocationTilePos = baseLocation;
                return;
            }
        }

    }
}

void StarterBot::doUpgrades()
{
    if (m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded()) return;

    for (auto& upgradeType : { BWAPI::UpgradeTypes::Protoss_Ground_Weapons, BWAPI::UpgradeTypes::Protoss_Ground_Armor, BWAPI::UpgradeTypes::Leg_Enhancements, BWAPI::UpgradeTypes::Singularity_Charge })
    {
        if (BWAPI::Broodwar->canUpgrade(upgradeType))
        {
            BWAPI::Unit unit = Tools::GetUnitOfType(upgradeType.whatUpgrades());
            if (unit) unit->upgrade(upgradeType);
            return;
        }
     
    }
}
       

bool isRefBuilt = false;
BWAPI::Unit lastZergling =  nullptr;

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{

    //if (!t1.joinable()) t1.detach();

    if (isZerglingRush)
    {
        BWAPI::Unit enemyUnit = nullptr;
        //for (size_t i=0; i< workers.size(); i++)
        //{
        //  
        //}

        enemyUnit = workers.getClosestUnit(BWAPI::Filter::IsEnemy, 1024);
        if ((!lastZergling && enemyUnit) || (enemyUnit && lastZergling && enemyUnit->getID() != lastZergling->getID()))
        {
            workers.attack(enemyUnit);
            defenders.attack(enemyUnit);
            lastZergling = enemyUnit;
        }
    }

    PROFILE_FUNCTION();

    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());

    for (auto& unit : myUnits)
    {

        // Get the closest mineral to this worker unit

        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle() && !unit->isMoving() && !unit->isAttacking())
        {
            //int numAssimilitorUnits = BWAPI::Broodwar->getUnitsOnTile(m_resourceManager.getRefineryResource(unit->getTilePosition().x, unit->getTilePosition().y).m_x, m_resourceManager.getRefineryResource(unit->getTilePosition().x, unit->getTilePosition().y).m_y).size();

            if (m_strategyManager.getBaseManager().getBaseofUnit(unit) > 0)
            {
                if (m_strategyManager.getBaseManager().getBuildingsCount(1, BWAPI::UnitTypes::Protoss_Nexus, true) <= 0) continue;

                if (Tools::CountBaseUnitssWithFilter(1, BWAPI::Filter::IsGatheringMinerals && BWAPI::Filter::GetPlayer, m_strategyManager.getBaseManager()) < 11)
                {
                    auto u = m_resourceManager.getMineralResource(m_mapTools.m_aStarPathFinding.m_baseLocations.m_allMinerals[1].m_x, m_mapTools.m_aStarPathFinding.m_baseLocations.m_allMinerals[1].m_y);
                    BWAPI::Unit closestMineral = BWAPI::Broodwar->getUnit(u.m_id);

                    if (closestMineral->getResources() == 0)
                    {
                        u = Tools::GetClosestResourceMineralToUnit(unit->getPosition());
                        closestMineral = BWAPI::Broodwar->getUnit(u.m_id);
                    }

                    if(closestMineral) unit->rightClick(closestMineral);
                }              
                else if(m_strategyManager.getBaseManager().getBuildingsCount(1, BWAPI::UnitTypes::Protoss_Assimilator, true) > 0)
                {
                    BWAPI::Unit assimilator = m_strategyManager.getBaseManager().getUnitOfTypeFromBase(1, BWAPI::UnitTypes::Protoss_Assimilator);
                    if (assimilator) unit->rightClick(assimilator);
                }
            }

            else
            {
                auto u = m_resourceManager.getMineralResource(BWAPI::Broodwar->self()->getStartLocation().x, BWAPI::Broodwar->self()->getStartLocation().y);
                BWAPI::Unit closestMineral = BWAPI::Broodwar->getUnit(u.m_id);

                if (Tools::CountBaseUnitssWithFilter(0,BWAPI::Filter::IsGatheringMinerals && BWAPI::Filter::GetPlayer, m_strategyManager.getBaseManager()) < 21)
                {
                    if (closestMineral->getResources() < 100)
                    {
                        u = Tools::GetClosestResourceMineralToUnit(unit->getPosition());
                        closestMineral = BWAPI::Broodwar->getUnit(u.m_id);
                    }

                    if (closestMineral) unit->rightClick(closestMineral);
                }
                else
                {
                    BWAPI::Unit refinery = m_strategyManager.getBaseManager().getUnitOfTypeFromBase(0, BWAPI::UnitTypes::Protoss_Assimilator);
                    if (refinery) unit->rightClick(refinery);
                }
            }

        }
    }
}

// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers()
{

    if (m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded()) return;

    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
  
    // get the unit pointer to my depot
    BWAPI::Unit myDepot = Tools::GetDepot();
    BWAPI::Unit mySecondDepot = Tools::GetDepot(1, m_strategyManager.getBaseManager());

    const int& numOfNexus = m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Nexus);

    if (myDepot)
    {
        const int workersOwned = m_strategyManager.getBaseManager().getNonBuildingsCount(0,workerType);
        const int workersWanted = m_strategyManager.getBaseManager().getBase(0).m_workersWanted;
        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (workersOwned < workersWanted && !myDepot->isTraining())
        { 
            myDepot->train(workerType); 
        }
    }
    if (mySecondDepot)
    {
        const int workersOwned = m_strategyManager.getBaseManager().getNonBuildingsCount(1, workerType);
        const int workersWanted = m_strategyManager.getBaseManager().getBase(1).m_workersWanted;
        if (workersOwned < workersWanted && numOfNexus > 1)
        {
            // if we have a valid depot unit and it's currently not training something, train a worker
            // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
            if (mySecondDepot && !mySecondDepot->isTraining()) { mySecondDepot->train(workerType); }
        }
    }
 
}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    //if (m_strategyManager.getBuildingStrategyManager().isBuildingBuiltNeeded()) return;

    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    PROFILE_FUNCTION();


    // If we have a sufficient amount of supply, we don't need to do anything
    if (!m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded() || BWAPI::Broodwar->self()->supplyUsed() >= 400)
    {
        return;
    }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    bool startedBuilding = Tools::BuildBuilding(supplyProviderType, m_strategyManager.getBuildingStrategyManager(),0, m_strategyManager.getBaseManager());
    if (startedBuilding)
    {
        m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded() = true;
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }

}


// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Nitro3D\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

BWAPI::TilePosition secondaryBasePylon = BWAPI::TilePositions::Invalid;

void StarterBot::buildBuildings()
{

    //std::cout << "My Units count: " << BWAPI::Broodwar->self()->allUnitCount() << std::endl;
    //std::cout << "Their Units count: " << BWAPI::Broodwar->enemy()->allUnitCount() << std::endl;

    if (m_strategyManager.getBuildingStrategyManager().isPylonRequired())
    {
        return;
    }

    if (m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Pylon) < 1 || m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded()) return;

    
    bool built = false;

    if (m_strategyManager.getBuildingStrategyManager().getWorkerID() > 0 && !BWAPI::Broodwar->isExplored(baseLocationTilePos))
    {
        if (BWAPI::Broodwar->getUnit(m_strategyManager.getBuildingStrategyManager().getWorkerID())->getLastCommand().getTargetTilePosition() != baseLocationTilePos)
        {
            BWAPI::Broodwar->getUnit(m_strategyManager.getBuildingStrategyManager().getWorkerID())->move(BWAPI::Position(baseLocationTilePos));
        }
    }

    m_strategyManager.getBaseManager().checkForInvalidMemory();

    if (!built && m_strategyManager.getBaseManager().getBasesMap().size() < 2 && m_strategyManager.getBuildingStrategyManager().getWorkerID() > 0 && m_strategyManager.getBaseManager().getBuildingsCount(1, BWAPI::UnitTypes::Protoss_Nexus, false) < 1 && baseLocationTilePos.isValid())
    {
        //if (Tools::checkIfBuildCommandAlreadyIssued(BWAPI::UnitTypes::Protoss_Nexus))
        //{
        //    return;
        //}

        if (BWAPI::Broodwar->getUnit(m_strategyManager.getBuildingStrategyManager().getWorkerID()))
        {
            m_strategyManager.getBuildingStrategyManager().getLastBuiltLocation(1) = baseLocationTilePos;
            BWAPI::Unit builder = BWAPI::Broodwar->getUnit(m_strategyManager.getBuildingStrategyManager().getWorkerID());
            baseLocationTilePos = m_strategyManager.getBuildingStrategyManager().getBuildingLocation(BWAPI::UnitTypes::Protoss_Nexus, builder,1);
            built = builder->build(BWAPI::UnitTypes::Protoss_Nexus, baseLocationTilePos);
        }

        if (built)
        {
            //numUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Gateway, BWAPI::Broodwar->self()->getUnits());
            //std::cout << "Gateway Built " << numUnits << std::endl;
            BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Nexus.c_str());
            //return;
            //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Gateway]= numUnits;
        }
    }

    std::vector<BWAPI::UnitType> buildOrderVector = m_strategyManager.getBuildingStrategyManager().getBuildingOrderVector();

    for (size_t i=0; i< buildOrderVector.size() && !built && m_strategyManager.getBuildingStrategyManager().getWorkerID() < 0; i++)
    {

        int mineralsLeft = BWAPI::Broodwar->self()->minerals();
        int mineralPrice = buildOrderVector[i].mineralPrice();
        int gasLeft = BWAPI::Broodwar->self()->gas();
        int gasPrice = buildOrderVector[i].gasPrice();
        int& numUnits = m_strategyManager.getNumberOfUnits(buildOrderVector[i]);


        if (Tools::checkIfBuildCommandAlreadyIssued(buildOrderVector[i]))
        {
            continue;
        }

        if (buildOrderVector[i] == BWAPI::UnitTypes::Protoss_Assimilator)
        {
            numUnits = m_strategyManager.getBaseManager().getBuildingsCount(0, buildOrderVector[i], false);
        }

        if (BWAPI::Broodwar->self()->isUnitAvailable(buildOrderVector[i]) && numUnits < m_strategyManager.getBuildingStrategyManager().getNumberOfBuildings(buildOrderVector[i]) && mineralsLeft >= mineralPrice && gasLeft >= gasPrice)
        {
            built = Tools::BuildBuilding(buildOrderVector[i], m_strategyManager.getBuildingStrategyManager(),0, m_strategyManager.getBaseManager());

            if (built)
            {
                //numUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Gateway, BWAPI::Broodwar->self()->getUnits());
                //std::cout << "Gateway Built " << numUnits << std::endl;
                BWAPI::Broodwar->printf("Started Building %s", buildOrderVector[i].getName().c_str());
                break;
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Gateway]= numUnits;
            }
        }
    }

    if (!built && m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Nexus) > 1 && !m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded())
    {
        int num = m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Nexus) - 1;
        BWAPI::Unit builder = m_strategyManager.getBaseManager().getWorkerFromBase(num);

        if (!builder) return;

        std::map<BWAPI::UnitType, int> map = m_strategyManager.getBuildingStrategyManager().getAdditionalBaseBuildingOrderMap();
        std::map<BWAPI::UnitType, int>::iterator it;

        bool built = false;

        for (it = map.begin(); it != map.end(); it++)
        {

            int mineralsLeft = BWAPI::Broodwar->self()->minerals();
            int mineralPrice = it->first.mineralPrice();
            int gasLeft = BWAPI::Broodwar->self()->gas();
            int gasPrice = it->first.gasPrice();
            int numUnits = m_strategyManager.getBaseManager().getBuildingsCount(num, it->first, false);


            if (it->first == BWAPI::UnitTypes::Protoss_Assimilator)
            {
                numUnits = m_strategyManager.getBaseManager().getBuildingsCount(num,it->first,false);
            }

            if (BWAPI::Broodwar->self()->isUnitAvailable(it->first) && numUnits < m_strategyManager.getBuildingStrategyManager().getSecondaryBaseNumberOfBuildings(it->first) && mineralsLeft >= mineralPrice && gasLeft >= gasPrice)
            {

                if (it->first != BWAPI::UnitTypes::Protoss_Pylon && m_strategyManager.getBaseManager().getBuildingsCount(num, BWAPI::UnitTypes::Protoss_Pylon, true) < 2)
                {
                    continue;
                }


                if (Tools::checkIfBuildCommandAlreadyIssued(it->first, builder->getID()) && it->first != BWAPI::UnitTypes::Protoss_Pylon)
                {
                    continue;
                }


                //if (it->first == BWAPI::UnitTypes::Protoss_Pylon && m_strategyManager.getBaseManager().getWorkerFromBase(1))
                //{
                //    builder = m_strategyManager.getBaseManager().getWorkerFromBase(1);
                //}

                Tools::BuildBuilding(it->first, m_strategyManager.getBuildingStrategyManager(), 1, m_strategyManager.getBaseManager());

                if (built)
                {
                    //numUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Gateway, BWAPI::Broodwar->self()->getUnits());
                    //std::cout << "Gateway Built " << numUnits << std::endl;
                    BWAPI::Broodwar->printf("Started Base 2 Building %s", it->first.getName().c_str());
                    break;
                    //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Gateway]= numUnits;
                }
                else
                {
                    secondaryBasePylon = BWAPI::TilePositions::Invalid;
                }
            }
        }
    }

}
bool isUnderAttack = false;
bool isScoutingAllowed = false;
int lastEnemyUnitID = -1;

void StarterBot::buildArmy()
{
    //if (m_strategyManager.getBuildingStrategyManager().isBuildingBuiltNeeded()) return;
    if (m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded()) return;
    if (m_strategyManager.getBuildingStrategyManager().getWorkerID() != -1) return;

    m_strategyManager.getUnitStrategyManager().trainCombatUnits(m_strategyManager.getUnitTypesCompletedMap());


    if (zealots.size() == 0) isScoutingAllowed = false;

    if (isUnderAttack)
    {
        isUnderAttack = false;

        //BWAPI::Unit enemeyUnit = zealots.getClosestUnit(BWAPI::Filter::IsEnemy, 512);
        for (auto it = zealots.begin(); it != zealots.end(); it++)
        {
            BWAPI::Unit u = *it;
            if (u && u->exists())
            {
                BWAPI::Unit enemyUnit = u->getClosestUnit(BWAPI::Filter::IsEnemy, 512);
                if (enemyUnit && u->getLastCommand().getTarget() && u->getLastCommand().getTarget()->getID() != enemyUnit->getID() && !u->getLastCommand().getTarget()->exists())
                {
                    u->attack(enemyUnit);
                    isUnderAttack = true;
                }
                else if (enemyUnit && u->getLastCommand().getTarget() && u->getLastCommand().getTarget()->getID() == enemyUnit->getID() && u->getLastCommand().getTarget()->exists())
                {
                    isUnderAttack = true;
                }
            }
        }
    }
    else if (!isUnderAttack && isScoutingAllowed)
    {
        bool isScoutingCompleted = true;

        //isUnderAttack = false;
        for (BWAPI::TilePosition pos : BWAPI::Broodwar->getStartLocations())
        {
            if (!BWAPI::Broodwar->isExplored(pos))
            {
                for (auto it = zealots.begin(); it != zealots.end(); it++)
                {
                    BWAPI::Unit u = *it;
                    if (u && u->exists())
                    {
                        BWAPI::Unit enemyUnit = u->getClosestUnit(BWAPI::Filter::IsEnemy, 512);
                        if (enemyUnit && (u->getLastCommand().getType() != BWAPI::UnitCommandTypes::Attack_Unit || (u->getLastCommand().getTarget() && !u->getLastCommand().getTarget()->exists() && u->getLastCommand().getTarget()->getID() != enemyUnit->getID())))
                        {
                            u->attack(enemyUnit);
                            isUnderAttack = true;
                        }
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
                isScoutingCompleted = false;
            }

        }

        if (!isScoutingCompleted)
        {
            isScoutingAllowed = true;
        }

    }


}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
    if (!unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        int& num = m_strategyManager.getNumberOfUnits(unit->getType());
        num--;

        int& completedNum = m_strategyManager.getNumberOfCompletedUnits(unit->getType());
        completedNum--;

        m_strategyManager.getUnitStrategyManager().m_killedUnitsCount[unit->getType()] += unit->getKillCount();
    }

    if (unit != nullptr && !unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) && (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot || unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon || unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar || unit->getType() == BWAPI::UnitTypes::Protoss_Corsair))
    {
        m_strategyManager.getUnitStrategyManager().m_deletedUnitsCount[unit->getType()] += 1;
    
        if (zealots.size() > 0)
        {
            zealots.erase(unit);
        }
        if (defenders.size() > 0)
        {
            defenders.erase(unit);
        }

        //for (auto it = defenders.begin(); it != defenders.end(); it++)
        //{
        //    BWAPI::Unit u = *it;
        //    if (unit->getID() == u->getID()) defenders.erase(*it);
        //}

        //for (auto it = zealots.begin(); it != zealots.end(); it++)
        //{
        //    BWAPI::Unit u = *it;
        //    if (unit->getID() == u->getID()) zealots.erase(*it);
        //}

    }
    if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        std::cout << "Enemy killed: " << unit->getType() <<std::endl;
    }

    if (!unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        m_strategyManager.getBaseManager().removeUnitFromBase(unit);
    }


}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{

    if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        return;
    }
    if (unit->getType().isRefinery())
    {
        int& numUnits = m_strategyManager.getNumberOfUnits(unit->getType());
        numUnits++;

        BWAPI::Unit builder = unit->getBuildUnit();

        if (unit->getType().isBuilding())
        {
            for (auto u : BWAPI::Broodwar->self()->getUnits())
            {
                if (u->isConstructing() && u->getType().isWorker())
                {
                    builder = u;
                    break;
                }
            }
        }
        std::cout << "Adding unit to the base" << m_strategyManager.getBaseManager().getBaseofUnit(builder) <<std::endl;

        m_strategyManager.getBaseManager().addUnitToBase(unit, builder);
    }


    //BWAPI::Broodwar->sendText("%s", unit->getType().c_str());
}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text)
{
    if (text == "map")
    {
        m_mapTools.toggleDraw();
    }

    //std::regex e("([0-9]+,[0-9]+)");

    //if (std::regex_match(text,e))
    {
        //char* cstr = new char(text.length()+1);
        //strcpy(cstr,text.c_str());
       // std::vector<char> cstr(text.c_str(), text.c_str()+text.size()+1);
        //std::cout << strtok(cstr.data(),",") << std::endl;
        //int n1 = atoi(strtok(cstr.data(), ","));
        //int n2 = atoi(strtok(NULL, ","));
        //AStarPathFinding star = AStarPathFinding();
        //star.startSearch(BWAPI::Broodwar->self()->getStartLocation(), BWAPI::TilePosition(n1,n2), m_strategyManager.getBuildingStrategyManager(), m_mapTools.m_walkable, m_mapTools.m_buildable);
    }


    BWAPI::Broodwar->sendText("%s", text.c_str());
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{
    if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        return;
    }
    
    int& numUnits = m_strategyManager.getNumberOfUnits(unit->getType());
    numUnits++;

    if (unit->getType() == BWAPI::UnitTypes::Protoss_Pylon)
    {
        m_strategyManager.getBuildingStrategyManager().isPylonRequired() = true;
    }

    if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus)
    {
        m_strategyManager.getBuildingStrategyManager().getLastBuiltLocation(numUnits - 1) = unit->getTilePosition();
        m_strategyManager.getBaseManager().addOrUpdateBase(unit, numUnits);
        //m_strategyManager.getBuildingStrategyManager().findCannonBuildingLocation(numUnits - 1);

        int& ID = m_strategyManager.getBuildingStrategyManager().getWorkerID();
        ID = -1;
        
        if (numUnits == 1)
        {
            m_strategyManager.getBaseManager().getBase(numUnits - 1).m_workersWanted = 20;
        }
        else
        {
            //defenders.move(BWAPI::Position(unit->getTilePosition()));
        }
       
    }
    else
    {
        BWAPI::Unit builder = unit->getBuildUnit();

        if (unit->getType().isBuilding())
        {
            for (auto u : BWAPI::Broodwar->self()->getUnits())
            {
                if (u->isConstructing() && u->getType().isWorker())
                {
                    builder = u;
                    break;
                }
            }
        }
        
        m_strategyManager.getBaseManager().addUnitToBase(unit, builder);
    }

    if (m_strategyManager.getBaseManager().getBuildingsCount(1, BWAPI::UnitTypes::Protoss_Nexus, true) && m_strategyManager.getBaseManager().getBuildingsCount(1, BWAPI::UnitTypes::Protoss_Pylon, false) >= 2)
    {
        //m_strategyManager.getBuildingStrategyManager().findCannonBuildingLocation(numUnits - 1);
        if (m_strategyManager.getBaseManager().getBuildingsCount(1, BWAPI::UnitTypes::Protoss_Photon_Cannon, false) == 3)
        {
            m_strategyManager.getBuildingStrategyManager().getBuildingOrderVector().push_back(BWAPI::UnitTypes::Protoss_Assimilator);
            m_strategyManager.getBuildingStrategyManager().getBuildingOrderMap()[BWAPI::UnitTypes::Protoss_Gateway]++;

            m_strategyManager.getBuildingStrategyManager().getBuildingOrderVector().push_back(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
            m_strategyManager.getBuildingStrategyManager().getBuildingOrderVector().push_back(BWAPI::UnitTypes::Protoss_Citadel_of_Adun);
            m_strategyManager.getBuildingStrategyManager().getBuildingOrderVector().push_back(BWAPI::UnitTypes::Protoss_Templar_Archives);
        }

    }

    /*if (m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon) == 3)
    {

    }*/
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{

    if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        return;
    }


    int& numCompletedUnits = m_strategyManager.getNumberOfCompletedUnits(unit->getType());
    numCompletedUnits++;

    if (unit->getType() == BWAPI::UnitTypes::Protoss_Pylon)
    {
        m_strategyManager.getBuildingStrategyManager().isAdditionalSupplyNeeded() = false;
        m_strategyManager.getBuildingStrategyManager().isPylonRequired() = false;
       
    }

    BWAPI::Unit worker = Tools::GetUnitOfType(BWAPI::UnitTypes::Protoss_Probe);
    for (auto& pos : m_strategyManager.getBuildingStrategyManager().m_cannonLocations[0])
    {
        if (!BWAPI::Broodwar->isExplored(pos))
        {
            worker->move(BWAPI::Position(pos));
            break;
        }
    }

    if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus && (numCompletedUnits > 1) )
    {
        m_strategyManager.getBaseManager().getBase(numCompletedUnits - 1).m_workersWanted = 10;
        m_strategyManager.getBuildingStrategyManager().findCannonBuildingLocation(numCompletedUnits - 1);
    }

    if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus && (numCompletedUnits == 1))
    {
        //m_strategyManager.getBaseManager().getBase(numCompletedUnits - 1).m_workersWanted = 10;
        m_strategyManager.getBuildingStrategyManager().findCannonBuildingLocation(0);
    }


    if (unit->getType().isBuilding())
    {
        if (unit->getType().isRefinery())
        {
            m_strategyManager.getBaseManager().checkForInvalidMemory();
            //m_strategyManager.getBaseManager().addUnitToBase(unit, m_strategyManager.getBaseManager().getBasesMap().size() - 1);
            int base = m_strategyManager.getBaseManager().getBaseofUnit(unit);
            m_strategyManager.getBaseManager().getBase(base).m_workersWanted += 3;
        }
    }

    else if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot || unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon || unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar)
    {
        if (defenders.size() < 5) 
        {
            defenders.emplace(unit);
        }
        else if ((zealots.size() < 10 && !isUnderAttack))
        {
            if(!isScoutingAllowed) zealots.emplace(unit);
            if (zealots.size() == 10)
            {
                isScoutingAllowed = true;
            }
        }
        else
        {
            defenders.emplace(unit);
        }

        if (m_strategyManager.getBaseManager().getBasesMap().size() > 1 && baseLocationTilePos.isValid() && m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Nexus) > 1)
        {
            int size = m_strategyManager.getBaseManager().getBasesMap()[1].m_buildings.size();
            BWAPI::Unit nexus = m_strategyManager.getBaseManager().getUnitOfTypeFromBase(1, BWAPI::UnitTypes::Protoss_Nexus);
            size = size == 0 ? 0 : size - 1;
            BWAPI::Unit lastBuildingBuilt = BWAPI::Broodwar->getUnit(m_strategyManager.getBaseManager().getBasesMap()[1].m_buildings[size]);
            if (nexus->getDistance(lastBuildingBuilt) <= 320)
            {
                baseLocationTilePos = lastBuildingBuilt->getTilePosition();
            }
            if(defenders.size() > 5) unit->move(BWAPI::Position(baseLocationTilePos));
        }
    }
    else if (unit->getType() == BWAPI::UnitTypes::Protoss_Probe)
    {
        workers.emplace(unit);
    }
   
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 

    if (!unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        return;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling && unit->getDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation())) < 640 && defenders.size() < 2 && zealots.size() < 2)
    {
        isZerglingRush = true;
    }
    else if (unit->getDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation())) < 640 || unit->getDistance(BWAPI::Position(baseLocationTilePos)) < 640)
    {
        if (unit->getDistance(zealots.getPosition()) < 640)
        {
            zealots.attack(unit);
        }

        defenders.attack(unit);
       
    }
}

void StarterBot::onUnitDiscover(BWAPI::Unit unit)
{


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

