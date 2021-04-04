#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include "../visualstudio/Profiler.hpp"

/**
* Nishant Rathore 
* Rohan Parmar
*/

std::thread& StarterBot::getThreadHandle()
{
    return std::ref(t1);
}

StarterBot::StarterBot(ResourceManager& rm)
{
    m_resourceManager = rm;
    m_strategyManager = StrategyManager();

}

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
    m_mapTools.onStart();

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

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    //createArmyBuildings();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}

////BFS Node
//struct BFSNode
//{
//    int x;
//    int y;
//    int position;
//    BFSNode* parent;
//
//    BFSNode(int x, int y, int position, BFSNode* parent)
//    {
//        this->x = x;
//        this->y = y;
//        this->position = position;
//        this->parent = parent;
//    }
//};
//
////Direction struct for BFS
//struct Direction
//{
//    int x = 0;
//    int y = 0;
//};
//
//struct Direction directions[4] = { {1,0},{0,-1},{-1,0},{0,1} }; // legal Directions for BFS 
//
//std::vector<BFSNode> openList; // BFS open list
//std::map<std::string,bool> closedList; // BFS closed map
//
//std::map<std::string, bool>::iterator it; // iterator to loop over closed list
//std::vector<BFSNode> path; // final path 
//
//
//
//// Order a move command to the workers to gather near the nexus
//void moveTrainedWorkers()
//{
//
//    BWAPI::TilePosition& pos = BWAPI::Broodwar->self()->getStartLocation();
//
//    openList.push_back(BFSNode(pos.x,pos.y,0,nullptr));
//
//    size_t size = openList.size();
//
//    for (size_t i=0; i<size; i++)
//    {
//        BFSNode node = openList[i];
//
//        it = closedList.find(std::to_string(node.x) + std::to_string(node.y));
//        if(it == closedList.end()) closedList.emplace(std::to_string(node.x)+ std::to_string(node.y),true);
//
//        for (size_t d = 0; d < 4; d++)
//        {
//            it = closedList.find(std::to_string(node.x + directions[d].x) + std::to_string(node.y + directions[d].y));
//            int x = node.x + directions[d].x;
//            int y = node.y + directions[d].y;
//
//            if (it == closedList.end())
//            {
//                BFSNode childNode = BFSNode(x, y, node.position++, &node);
//                bool arePositionsFound = true;
//                path.push_back(childNode);
//                for (size_t c = 0; c < 7; c++)
//                {         
//                    x = x + directions[d].x;
//                    y = y + directions[d].y;
//
//                    if (BWAPI::Broodwar->getUnitsOnTile(x,y).size())
//                    {
//                        arePositionsFound = false;
//                        break;
//                    }
//                    path.push_back(BFSNode(x, y, c+1, &childNode));
//                }
//
//                // if the desired path is found, stop BFS and return
//                if (arePositionsFound && path.size() == 8)
//                {
//                    return;
//                }
//                // cleat vector for the next iteration
//                path.clear();
//                // add to open list
//                openList.push_back(childNode);
//                size++;
//            }
//        } 
//
//    }
//
//}

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
            BWAPI::Unit closestMineral = Tools::GetClosestResourceMineralToUnit(pos);
            BWAPI::Unit closestGeyesers = Tools::GetClosestGeyserToUnit(pos);
            setMineralResource(j, i, closestMineral);
            setRefineryResource(j, i, closestGeyesers);
        }
    }

    isJobComplete = true;
}

bool isRefBuilt = false;
bool isGatewayBuilt = false;
bool isForgeBuilt = false;
bool isCannonBuilt = false;

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{

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

        if (!u) continue;
        if (u->getID() == -1) continue;

        BWAPI::Position pos(BWAPI::TilePosition(unit->getTilePosition().x, unit->getTilePosition().y));
        BWAPI::Unit closestMineral = u;

       // int closMinID = closestMineral->getID();

        //std::string t = std::to_string(closestMineral->getTilePosition().x) + " " + std::to_string(closestMineral->getTilePosition().y);

        BWAPI::Unit closestGeyser = m_resourceManager.getRefineryResource(unit->getTilePosition().x, unit->getTilePosition().y);

        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle() && !unit->isMoving() && !unit->isAttacking())
        {

            int mineralsLeft = BWAPI::Broodwar->self()->minerals();
            //
            int mineralPrice = BWAPI::Broodwar->self()->getRace().getRefinery().mineralPrice();

            int numAssimilitorUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Assimilator, BWAPI::Broodwar->self()->getUnits());
            //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Assimilator] = numAssimilitorUnits;
            int& numAssimiltorCompletedUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Assimilator);
            numAssimiltorCompletedUnits = numAssimilitorUnits;
            if (mineralsLeft >= mineralPrice && numAssimilitorUnits < 1)
            {

                isRefBuilt = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Assimilator, m_strategyManager.getBuildingStrategyManager());
                //std::cout << "Refinery Built" << std::endl;
                if (isRefBuilt)
                {
                    //BWAPI::TilePosition lastBuiltLocation = m_strategyManager.getBuildingStrategyManager().getLastBuiltLocation();
                    //numUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Assimilator, BWAPI::Broodwar->self()->getUnits());
                    //numUnits++;
                    //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Assimilator] = numUnits;
                    
          
                }
                if (closestMineral)
                {
                    unit->rightClick(closestMineral);
                }
            }

            else if (closestMineral) { unit->rightClick(closestMineral); }
            
            //mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();
            ////
            //mineralPrice = BWAPI::Broodwar->self()->getRace().getRefinery().mineralPrice();

            //if (mineralsLeft - mineralPrice > 0 && !isGatewayBuilt)
            //{

            //    isGatewayBuilt = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Gateway);

            //    isGatewayBuilt = true;
            //}

            //mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();
            ////
            //mineralPrice = BWAPI::UnitTypes::Protoss_Forge;


            //if (mineralsLeft - mineralPrice > 0 && !isForgeBuilt)
            //{
            //    isForgeBuilt = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Forge);
            //    isForgeBuilt = true;
            //}


            //mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();
            ////
            //mineralPrice = BWAPI::UnitTypes::Protoss_Photon_Cannon;


            //if (mineralsLeft - mineralPrice > 0)
            //{
            //    Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Photon_Cannon);
            //}

            //    if (true)//unit->build(BWAPI::Broodwar->self()->getRace().getRefinery(), closestGeyser->getTilePosition()))
            //    {

            //    }
            //    else
            //    {
            //        if (refineryPosition.x > 0 && refineryPosition.y > 0)
            //        {
            //            for (const BWAPI::Unit& u : BWAPI::Broodwar->getUnitsOnTile(refineryPosition))
            //            {
            //                if (!unit->rightClick(u))
            //                {

            //                    if (closestMineral) { unit->rightClick(closestMineral); }
            //                }
            //            }
            //        }
            //        else
            //        {
            //            if (closestMineral) { unit->rightClick(closestMineral); }
            //        }
            //    }

            //}
            //else
            //{
            //    if (closestMineral) { unit->rightClick(closestMineral); }
            //}
          
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
    else
    {

        int& numUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Gateway);
        int& numForgeUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Forge);

        int& numDragoonUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Cybernetics_Core);

        int& numCannonUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);

        int& citadelOfAdunUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Citadel_of_Adun);

        int& templarArchiveUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Templar_Archives);


        int mineralsLeft = BWAPI::Broodwar->self()->minerals();

        int mineralPrice = BWAPI::UnitTypes::Protoss_Gateway.mineralPrice();

        if (numUnits < 2 && mineralsLeft >= mineralPrice)
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Gateway, m_strategyManager.getBuildingStrategyManager());

            if (built)
            {
                //numUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Gateway, BWAPI::Broodwar->self()->getUnits());
                //std::cout << "Gateway Built " << numUnits << std::endl;
                BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Gateway.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Gateway]= numUnits;
            }
        }
        
        mineralsLeft = BWAPI::Broodwar->self()->minerals();

        mineralPrice = BWAPI::UnitTypes::Protoss_Forge.mineralPrice();

        if (numForgeUnits < 1 && mineralsLeft >= mineralPrice && m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Gateway))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Forge, m_strategyManager.getBuildingStrategyManager());

            if (built)
            {
                //numForgeUnits++;
                //numForgeUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Forge, BWAPI::Broodwar->self()->getUnits());
                BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Forge.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Forge] = numForgeUnits;
                //int t = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Forge);
            }
        }

        mineralsLeft = BWAPI::Broodwar->self()->minerals();

        mineralPrice = BWAPI::UnitTypes::Protoss_Cybernetics_Core.mineralPrice();

        if (numDragoonUnits < 1 && (mineralsLeft >= mineralPrice) && m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Gateway))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Cybernetics_Core, m_strategyManager.getBuildingStrategyManager());

            if (built)
            {
                //numDragoonUnits++;
                //numDragoonUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Cybernetics_Core, BWAPI::Broodwar->self()->getUnits());
                //std::cout << "Cybernetics_Core Built:  " << numDragoonUnits << std::endl;
                BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Cybernetics_Core.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Cybernetics_Core] = numDragoonUnits;
                //int t = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
            }
        }



        mineralsLeft = BWAPI::Broodwar->self()->minerals();

        mineralPrice = BWAPI::UnitTypes::Protoss_Photon_Cannon.mineralPrice();

        if (numCannonUnits < 10 && mineralsLeft >= mineralPrice && m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Forge))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Photon_Cannon, m_strategyManager.getBuildingStrategyManager());
            
            if (built)
            {
                //numCannonUnits++;
                //numCannonUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Photon_Cannon, BWAPI::Broodwar->self()->getUnits());
                BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Photon_Cannon.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Photon_Cannon] = numCannonUnits;
                //int t = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);
            }
        }

        mineralsLeft = BWAPI::Broodwar->self()->minerals();

        mineralPrice = BWAPI::UnitTypes::Protoss_Citadel_of_Adun.mineralPrice();

        int gasLeft = BWAPI::Broodwar->self()->gas();

        int gasPrice = BWAPI::UnitTypes::Protoss_Citadel_of_Adun.gasPrice();

        if (citadelOfAdunUnits < 1 && mineralsLeft >= mineralPrice && gasLeft >= gasPrice && m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Cybernetics_Core))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, m_strategyManager.getBuildingStrategyManager());

            if (built)
            {
                //numCannonUnits++;
                //citadelOfAdunUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, BWAPI::Broodwar->self()->getUnits());
                BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Citadel_of_Adun.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Citadel_of_Adun] = citadelOfAdunUnits;
                //int t = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);
            }
        }

        mineralsLeft = BWAPI::Broodwar->self()->minerals();

        mineralPrice = BWAPI::UnitTypes::Protoss_Templar_Archives.mineralPrice();

        gasLeft = BWAPI::Broodwar->self()->gas();

        gasPrice = BWAPI::UnitTypes::Protoss_Templar_Archives.gasPrice();

        if (templarArchiveUnits < 1 && mineralsLeft >= mineralPrice && gasLeft >= gasPrice && m_strategyManager.getNumberOfCompletedUnits(BWAPI::UnitTypes::Protoss_Citadel_of_Adun))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Templar_Archives, m_strategyManager.getBuildingStrategyManager());

            if (built)
            {
                //numCannonUnits++;
                //templarArchiveUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Templar_Archives, BWAPI::Broodwar->self()->getUnits());
                BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Templar_Archives.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Templar_Archives] = templarArchiveUnits;
                //int t = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Photon_Cannon);
            }
        }

        // sets the flag so that workers are not sent back mining
        // once enough units are created, order a move commands
        //moveTrainedWorkers();

        //for (auto& unit: BWAPI::Broodwar->self()->getUnits())
        //{
        //    // check if it is the right unit that needs moved
        //    if (unit->isGatheringMinerals() && unit->isCompleted() && unit->getType() == workerType)
        //    {
        //        if(count >= 0)
        //        {
        //            BWAPI::Position pos = BWAPI::Position(BWAPI::TilePosition(path[count].x, path[count].y));
        //            auto command = unit->getLastCommand();
        //            // if the last command is not the same as this position, move this unit
        //            if (command.getTargetPosition() != pos)  unit->move(pos);
        //            count--;
        //        }
        //    }
        //}

    }
}

BWAPI::Unitset zealots;
BWAPI::Unitset defenders;
BWAPI::Unitset workers;
BWAPI::Position unitPosition = BWAPI::Positions::Invalid;
BWAPI::UnitCommandType zealotsLastCommandType = BWAPI::UnitCommandTypes::None;
int lastEnemeyAttackingID = -1;

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
        
    PROFILE_FUNCTION();

    BWAPI::Unit builder = Tools::GetTrainerUnitNotFullOfType(BWAPI::UnitTypes::Protoss_Zealot);
    if (builder)
    {
        if(!Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Templar_Archives, BWAPI::Broodwar->self()->getUnits()))
        {
            if (Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot, BWAPI::Broodwar->self()->getUnits()) <= 3)
            {
                builder->train(BWAPI::UnitTypes::Protoss_Zealot);
            }
            if (Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Dragoon, BWAPI::Broodwar->self()->getUnits()) <= 2)
            {
                builder->train(BWAPI::UnitTypes::Protoss_Dragoon);
            }
        }
        else
        {
            builder->train(BWAPI::UnitTypes::Protoss_Dark_Templar);
        }
    }

    bool isUnderAttack = false;

    if(zealots.size() == 10)
    {

        for (BWAPI::TilePosition pos : BWAPI::Broodwar->getStartLocations())
        {
            int count = 0;

            for (auto it = zealots.begin(); it!= zealots.end() && count < 10;it++)
            {
                count++;
                if (!BWAPI::Broodwar->isExplored(pos))
                {
                    BWAPI::Unit unit = *it;
                    if (unit->isUnderAttack()) 
                    {
                        unitPosition = unit->getPosition();
                        isUnderAttack = true;
                        break;
                    }
                }
            }
            if (isUnderAttack)
            {
                break;
            }
            else if (unitPosition != BWAPI::Position(pos) && !BWAPI::Broodwar->isExplored(pos))
            {
                zealots.move(BWAPI::Position(pos));
                unitPosition = BWAPI::Position(pos);
            }
      
        }

        BWAPI::Unit enemeyUnit = BWAPI::Broodwar->getClosestUnit(unitPosition, BWAPI::Filter::IsEnemy);
        if (enemeyUnit && enemeyUnit->getID() != lastEnemeyAttackingID)
        {
            zealots.attack(enemeyUnit);
            lastEnemeyAttackingID = enemeyUnit->getID();
            unitPosition = BWAPI::Positions::Invalid;
        }
    }


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
  

    //int mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();

    //int mineralPrice = BWAPI::UnitTypes::Protoss_Forge.mineralPrice();

    //if (mineralsLeft - mineralPrice > 0)
    //{
    //    startedBuilding = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Forge);

    //}

    //if (startedBuilding)
    //{
    //    BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Forge.getName().c_str());
    //}

    //mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();

    //mineralPrice = BWAPI::UnitTypes::Protoss_Photon_Cannon.mineralPrice();

    //if (mineralsLeft - mineralPrice)
    //{
    //    startedBuilding = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Photon_Cannon);
    //}

    //if (startedBuilding)
    //{
    //    BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Photon_Cannon.getName().c_str());
    //}

    //mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();

    //mineralPrice = BWAPI::UnitTypes::Protoss_Photon_Cannon.mineralPrice();

    //startedBuilding = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Photon_Cannon);

}


// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "RathoreParmar\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
    if (unit->getType() == BWAPI::UnitTypes::Protoss_Photon_Cannon)
    {
        BWAPI::Unit worker = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());
        if (worker->build(unit->getType(), BWAPI::TilePosition(unit->getPosition())))
        {
            BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Photon_Cannon.getName().c_str());
        }
    }
    if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot || unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon || unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar)
    {
        defenders.erase(unit);
        zealots.erase(unit);
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

    if (!unit->getType().isBuilding())
    {
        BWAPI::Unit builder = unit->getBuildUnit();
        int& numUnits = m_strategyManager.getNumberOfUnits(unit->getType());
        if (numUnits < m_strategyManager.getUnitStrategyManager().getNumberOfUnits(unit->getType()))
        {
            //numUnits++;
            if (builder->train(unit->getType())) m_strategyManager.getUnitTypesMap()[unit->getType()] = numUnits;
            numUnits = Tools::CountUnitsOfType(unit->getType(), BWAPI::Broodwar->self()->getUnits());
            std::cout << "Total zealots: " << numUnits << std::endl;
        }
    }

}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
    int& numCompletedUnits = m_strategyManager.getNumberOfCompletedUnits(unit->getType());
    numCompletedUnits++;

    if (m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Assimilator] && unit->getType().isWorker())
    {
        unit->rightClick(Tools::GetUnitOfType(BWAPI::UnitTypes::Protoss_Assimilator));
    }

    if (unit->getType().isBuilding())
    {
        if (unit->getType().buildsWhat().count(BWAPI::UnitTypes::Protoss_Zealot))
        {
            int& numUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Zealot);
            int& numDragoonUnits = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Dragoon);
            if (numUnits < m_strategyManager.getUnitStrategyManager().getNumberOfUnits(BWAPI::UnitTypes::Protoss_Zealot))
            {
                //numUnits++;
                if (unit->train(BWAPI::UnitTypes::Protoss_Zealot)) m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Zealot] = numUnits;
                numUnits = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot,BWAPI::Broodwar->self()->getUnits());
            }
            if (numDragoonUnits < m_strategyManager.getUnitStrategyManager().getNumberOfUnits(BWAPI::UnitTypes::Protoss_Dragoon))
            {
                //numDragoonUnits++;
                if (unit->train(BWAPI::UnitTypes::Protoss_Dragoon)) m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Dragoon] = numDragoonUnits;
                numUnits = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Dragoon, BWAPI::Broodwar->self()->getUnits());
            }
        }
        else if (unit->getType().isRefinery())
        {
            Tools::GetDepot()->train(BWAPI::UnitTypes::Protoss_Probe);
            Tools::GetDepot()->train(BWAPI::UnitTypes::Protoss_Probe);
            Tools::GetDepot()->train(BWAPI::UnitTypes::Protoss_Probe);
        }
    }

    if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot || unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon || unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar)
    {
        if (defenders.size() < 5) 
        {
            defenders.emplace(unit);
        }
        else if (zealots.size() < 10) 
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
    //std::cout << unit->getType().getName();


    if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        if (unit->getDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation())) < 1000) 
        {
            defenders.attack(unit);
        }
    }
    else if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) && unit->getType() == BWAPI::UnitTypes::Zerg_Zergling)
    {
        workers.attack(unit);
    }

    //if (t1.joinable()) t1.join();
    if (unit->getType().isMineralField())
    {
        if (resourceCount == 100)
        {
            //std::cout << "Count is 7" << std::endl;
            if (!t1.joinable()) 
            {
                resourceCount = 0;
                m_resourceManager.isJobComplete = false;
                t1 = std::thread(&ResourceManager::calculateDistances, std::ref(m_resourceManager));
            }
        }
        else
        {
            if (t1.joinable() && m_resourceManager.isJobComplete) 
            {
                t1.join();
                resourceCount = 0;
            }
            ++resourceCount;
        }
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

ResourceManager::ResourceManager()
{
    int m_width = BWAPI::Broodwar->mapWidth();
    int m_height = BWAPI::Broodwar->mapHeight();
    m_mineralResource = Grid<BWAPI::Unit>(m_width, m_height, BWAPI::Unit());
    m_refineryResource = Grid<BWAPI::Unit>(m_width, m_height, BWAPI::Unit());
}

BWAPI::Unit ResourceManager::getMineralResource(int x, int y)
{
    return m_mineralResource.get(x, y);
}

void ResourceManager::setMineralResource(int x, int y, BWAPI::Unit unit)
{
    m_mineralResource.set(x, y, unit);
}

BWAPI::Unit ResourceManager::getRefineryResource(int x, int y)
{
    return m_refineryResource.get(x, y);
}

void ResourceManager::setRefineryResource(int x, int y, BWAPI::Unit unit)
{
    m_refineryResource.set(x, y, unit);
}
