#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include "../visualstudio/Profiler.hpp"

/**
* Nishant Rathore 
* Rohan Parmar
*/

StarterBot::StarterBot(ResourceManager& rm)
{
    m_resourceManager = rm;
    m_strategyManager = StrategyManager();

}

// Called when the bot starts!
void StarterBot::onStart()
{
    count = 7;
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
    //BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);

    // Call MapTools OnStart
    m_mapTools.onStart();

}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner) 
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

int gatewayCount = 100;

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
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(pos, BWAPI::Broodwar->getMinerals());
            BWAPI::Unit closestGeyesers = Tools::GetClosestUnitTo(pos, BWAPI::Broodwar->getGeysers());
            setMineralResource(j, i, closestMineral);
            setRefineryResource(j, i, closestGeyesers);
        }
    }
}

bool isRefBuilt = false;
bool isGatewayBuilt = false;
bool isForgeBuilt = false;
bool isCannonBuilt = false;

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{

    PROFILE_FUNCTION();

    if (count > 5)
    {
       // m_resourceManager.calculateDistances();

        if(t1.joinable()) t1.join();

        t1 = std::thread(&ResourceManager::calculateDistances, std::ref(m_resourceManager));

        //for(const BWAPI::Unit& u : BWAPI::Broodwar->getGeysers()) std::cout << u->getTilePosition() << std::endl << " Done "<<std::endl;
        //for (const BWAPI::Unit& u : BWAPI::Broodwar->getMinerals()) std::cout << u->getTilePosition() << std::endl;

        //BWAPI::UnitType ut = BWAPI::UnitTypes::Protoss_Gateway;

        //for (auto u : ut.requiredUnits())        {
        //    std::cout << u.first << "   " << u.second << std::endl;
        //}

        count--;
    }
    else 
    {
        if (t1.joinable())
        {
            t1.detach();
        }
    }
   

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
        if (unit->getType().isWorker() && unit->isIdle())
        {

            int mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();
            //
            int mineralPrice = BWAPI::Broodwar->self()->getRace().getRefinery().mineralPrice();

            if (mineralsLeft - mineralPrice > 0 && !m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Assimilator])
            {

                Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Assimilator);
                //std::cout << "Refinery Built" << std::endl;
                //if (isRefBuilt) m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Assimilator] = 1;
            }

            else if (m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Assimilator])
            {
                u = m_resourceManager.getRefineryResource(unit->getTilePosition().x, unit->getTilePosition().y);
                if (u && u->getID() != -1) { unit->rightClick(u); }
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
    const int workersWanted = 10;
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


        int mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();

        int mineralPrice = BWAPI::UnitTypes::Protoss_Gateway.mineralPrice();

        if (numUnits < 2 && (mineralsLeft - mineralPrice))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Gateway);

            if (built)
            {
                //numUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Gateway, BWAPI::Broodwar->self()->getUnits());
                //std::cout << "Gateway Built " << numUnits << std::endl;
                BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Gateway.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Gateway]= numUnits;
            }
        }
        
        mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();

        mineralPrice = BWAPI::UnitTypes::Protoss_Forge.mineralPrice();

        if (numForgeUnits < 1 && (mineralsLeft - mineralPrice))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Forge);

            if (built)
            {
                //numForgeUnits++;
                //numForgeUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Forge, BWAPI::Broodwar->self()->getUnits());
                BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Forge.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Forge] = numForgeUnits;
                //int t = m_strategyManager.getNumberOfUnits(BWAPI::UnitTypes::Protoss_Forge);
            }
        }

        mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();

        mineralPrice = BWAPI::UnitTypes::Protoss_Cybernetics_Core.mineralPrice();

        if (numDragoonUnits < 1 && (mineralsLeft - mineralPrice))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Cybernetics_Core);

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



        mineralsLeft = BWAPI::Broodwar->self()->gatheredMinerals() - BWAPI::Broodwar->self()->spentMinerals();

        mineralPrice = BWAPI::UnitTypes::Protoss_Photon_Cannon.mineralPrice();

        if (numCannonUnits < 8 && (mineralsLeft - mineralPrice))
        {
            bool built = Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Photon_Cannon);

            if (built)
            {
                //numCannonUnits++;
                //numCannonUnits = Tools::CountBuildingUnitsOfType(BWAPI::UnitTypes::Protoss_Photon_Cannon, BWAPI::Broodwar->self()->getUnits());
                //BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Protoss_Photon_Cannon.getName().c_str());
                //m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Photon_Cannon] = numCannonUnits;
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

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{

    PROFILE_FUNCTION();

    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) 
    {
        return;
    }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
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
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{
    if (m_strategyManager.getUnitTypesMap()[BWAPI::UnitTypes::Protoss_Assimilator] && unit->getType().isWorker())
    {
        unit->rightClick(Tools::GetUnitOfType(BWAPI::UnitTypes::Protoss_Assimilator));
    }
    
    if (unit->getType().isBuilding())
    {
        int& numUnits = m_strategyManager.getNumberOfUnits(unit->getType());
        numUnits = Tools::CountBuildingUnitsOfType(unit->getType(), BWAPI::Broodwar->self()->getUnits());
    }

}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
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

    //if (t1.joinable()) t1.join();

    //t1 = std::thread(&ResourceManager::calculateDistances, std::ref(m_resourceManager));
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
