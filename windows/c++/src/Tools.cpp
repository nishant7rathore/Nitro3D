#include "Tools.h"
#include "AStarPathFinding.h"

BWAPI::TilePosition lastSetPylonTilePosition = BWAPI::TilePositions::Invalid;

BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units)
{
    BWAPI::Unit closestUnit = nullptr;

    for (auto& u : units)
    {
        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p))
        {
            closestUnit = u;
        }
    }

    return closestUnit;
}

BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units)
{
    if (!unit) { return nullptr; }
    return GetClosestUnitTo(unit->getPosition(), units);
}

Resource Tools::GetClosestResourceMineralToUnit(BWAPI::Position p)
{
    BWAPI::Unit closestUnit = nullptr;

    for (auto& u : BWAPI::Broodwar->getStaticNeutralUnits())
    {
        if (!u->getType().isMineralField()) continue;

        if (u->getResources() < 10) continue;

        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p))
        {
            closestUnit = u;
        }
    }
    return Resource(closestUnit->getID(),closestUnit->getTilePosition().x, closestUnit->getTilePosition().y, closestUnit->getInitialResources(), closestUnit->getDistance(p), false);
}


std::vector<Resource> Tools::GetAllMinerals(BWAPI::Position p)
{
    std::vector<Resource> allMinerals;

    for (auto& unit : BWAPI::Broodwar->getStaticNeutralUnits())
    {
        if (unit->getType() != BWAPI::UnitTypes::Resource_Vespene_Geyser) continue;

        allMinerals.push_back(Resource(unit->getID(), unit->getTilePosition().x, unit->getTilePosition().y, unit->getInitialResources(), unit->getDistance(p), false));
    }

    return allMinerals;
}

std::vector<BWAPI::TilePosition> Tools::GetBaseLocationsList(std::vector<Resource>& allMineralsList, BuildingStrategyManager& bm)
{
    BWAPI::Unit resourceDepot = Tools::GetDepot();
    std::vector<Resource> resourceList;
    std::vector<BWAPI::TilePosition> baseLocations;
    Resource currentResource;

    int distance = INT_MAX;

    for (auto& res : allMineralsList)
    {
        currentResource = Resource();
        distance = INT_MAX;

        for (auto& resource : allMineralsList)
        {
            if (isResourceInOurList(resource, resourceList)) continue;
            int newDistance = resourceDepot->getDistance(BWAPI::Position(BWAPI::TilePosition(resource.m_x, resource.m_y)));

            if (newDistance < distance)
            {
                distance = newDistance;
                currentResource = resource;
            }
        }

        if (!isResourceInOurList(currentResource, resourceList))
        {
            BWAPI::TilePosition tilePos = bm.getBuildingLocation(BWAPI::UnitTypes::Protoss_Nexus, BWAPI::TilePosition(currentResource.m_x, currentResource.m_y));
            if (tilePos.isValid())
            {
                baseLocations.push_back(BWAPI::TilePosition(currentResource.m_x, currentResource.m_y));
                //baseLocations.push_back(tilePos);
            }
            resourceList.push_back(currentResource);
        }
    }
    
   //if(baseLocations.size() >= 1)  baseLocations[0] = resourceDepot->getTilePosition();

    return baseLocations;
    
}


bool Tools::isResourceInOurList(Resource& resource, std::vector<Resource>& resourceList)
{
    std::vector<Resource>::iterator it;

    for (it = resourceList.begin(); it != resourceList.end(); it++)
    {
        if (resource.m_id == it->m_id)
        {
            return true;
        }
    }

    return false;
}


bool Tools::isMineralInOurList(BWAPI::Unit mineral, std::vector<Resource>& resourceList)
{
    std::vector<Resource>::iterator it;

    for (it = resourceList.begin(); it != resourceList.end(); it++)
    {
        if (mineral->getID() == it->m_id)
        {
            return true;
        }

        //check if any of the units in the range are on the list
        for (auto unit : mineral->getUnitsInRadius(544, BWAPI::Filter::IsNeutral))
        {
            if (unit->getID() == it->m_id && unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field)
            {
                return true;
            }
        }
    }

    return false;
}

Resource Tools::GetClosestGeyserToUnit(BWAPI::Position p)
{
    BWAPI::Unit closestUnit = nullptr;

    for (auto& u : BWAPI::Broodwar->getStaticNeutralUnits())
    {
        if (u->getType() != BWAPI::UnitTypes::Resource_Vespene_Geyser) continue;

        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p))
        {
            closestUnit = u;
        }
    }

    return Resource(closestUnit->getID(),closestUnit->getTilePosition().x, closestUnit->getTilePosition().y, closestUnit->getInitialResources(), closestUnit->getDistance(p),true);
}

int Tools::CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units)
{
    int sum = 0;
    for (auto& unit : units)
    {
        if (unit->getType() == type)
        {
            sum++;
        }
    }

    return sum;
}

int Tools::CountUnitsOfType(BWAPI::UnitType type, bool isUnderConstruction)
{
    const BWAPI::Unitset& units = BWAPI::Broodwar->self()->getUnits();
    int sum = 0;
    for (auto& unit : units)
    {
        if (unit->getType() == type && (!unit->isCompleted() && isUnderConstruction))
        {
            sum++;
        }
    }

    return sum;
}

int Tools::CountBaseUnitssWithFilter(int base, BWAPI::UnitFilter filter, BaseManager& bm)
{
    BWAPI::Unit baseUnit = bm.getBasesMap()[base].m_base;

    if (!baseUnit || !baseUnit->exists()) return 0;

    size_t size = 0;

    for (size_t i = 0; i < bm.getBasesMap()[base].m_workers.size(); i++)
    {
        BWAPI::Unit worker = BWAPI::Broodwar->getUnit(bm.getBasesMap()[base].m_workers[i]);

        if (worker->getType() == BWAPI::UnitTypes::Protoss_Probe && worker->isGatheringMinerals())
        {
            size++;
        }
    }

    return size;
}

int Tools::CountBuildingUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units)
{
    int sum = 0;
    for (auto& unit : units)
    {
        if (unit->getType() == type)
        {
            sum++;
        }
    }

    return sum;
}

BWAPI::Unit Tools::GetUnitOfType(BWAPI::UnitType type)
{
    // For each unit that we own
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if the unit is of the correct type, and it actually has been constructed, return it
        if (unit->getType() == type && unit->isCompleted())
        {
            return unit;
        }
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}

BWAPI::Unit Tools::GetWorkerExcluding(int ID, int base, BaseManager& bm)
{

    for (size_t i = 0; i < bm.getBasesMap()[base].m_workers.size(); i++)
    {
        int uID = bm.getBasesMap()[base].m_workers[i];
        BWAPI::Unit unit = ID == uID ? nullptr : BWAPI::Broodwar->getUnit(uID);
        if (unit && unit->exists() && unit->isCompleted() && unit->getType().isWorker())
        {
            return unit;
        }
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}


BWAPI::Unit Tools::GetBuilderNotBuildingCurrentlyOfType(BWAPI::UnitType type)
{
    // For each unit that we own
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if the unit is of the correct type, and it actually has been constructed, return it
        if (unit->getType() == type && unit->isCompleted() && unit->getLastCommand().getType() != BWAPI::UnitCommandTypes::Build)
        {
            return unit;
        }
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}


BWAPI::Unit Tools::GetTrainerUnitNotFullOfType(BWAPI::UnitType type)
{
    // For each unit that we own
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if the unit is of the correct type, and has a room to train this unit type, return it
        if (unit->getType() == type.whatBuilds().first && !unit->isTraining())
        {
            return unit;
        }

        // if the unit is of the correct type, and has a room to train this unit type, return it
        if (unit->getType() == type.whatBuilds().first && unit->getTrainingQueue().size() < 5)
        {
            return unit;
        }
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}

BWAPI::Unit Tools::GetIdleBuilder()
{
    // For each builder that we own
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if the worker's last command was not build, and it actually has been constructed, return it
        if (unit->getType().isWorker() && unit->isCompleted())
        {
            return unit;
        }
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}

BWAPI::Unit Tools::GetDepot(int base, BaseManager& bm)
{
    const BWAPI::UnitType depot = BWAPI::Broodwar->self()->getRace().getResourceDepot();
    return bm.getUnitOfTypeFromBase(base,depot);
}

BWAPI::Unit Tools::GetDepot()
{
    const BWAPI::UnitType depot = BWAPI::Broodwar->self()->getRace().getResourceDepot();
    return Tools::GetUnitOfType(depot);
}

// Attempt tp construct a building of a given type 
bool Tools::BuildBuilding(BWAPI::UnitType type, BuildingStrategyManager& bsm, int base, BaseManager& bm)
{
    // Get the type of unit that is required to build the desired building
    BWAPI::UnitType builderType = type.whatBuilds().first;

    // Get a unit that we own that is of the given type so it can build
    // If we can't find a valid builder unit, then we have to cancel the building
    BWAPI::Unit builder = Tools::GetWorkerExcluding(bsm.getWorkerID(),base, bm);
    if (!builder) { return false; }

    // Get a location that we want to build the building next to
    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();

    // Ask BWAPI for a building location near the desired position for the type
    int maxBuildRange = 64;
    bool buildingOnCreep = type.requiresCreep();

    bool hasBuilt = false;

    BWAPI::TilePosition pos = BWAPI::TilePositions::Invalid;

    if (type == BWAPI::UnitTypes::Protoss_Pylon)
    {
        if (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Pylon.mineralPrice())
        {
            pos = bsm.getBuildingLocation(type, builder, base);
            lastSetPylonTilePosition = pos;
            hasBuilt = builder->build(type, lastSetPylonTilePosition);
            /*if (lastSetPylonTilePosition == BWAPI::TilePositions::Invalid || BWAPI::Broodwar->getUnitsOnTile(lastSetPylonTilePosition).size() > 0)
            {
               
            }*/
   
        }
    }
    else
    {
        if (!bsm.isAdditionalSupplyNeeded())
        {
            pos = bsm.getBuildingLocation(type, builder,base);

            if (pos == BWAPI::TilePositions::None)
            {
                //bsm.isAdditionalSupplyNeeded() = true;
                pos = BWAPI::Broodwar->getBuildLocation(BWAPI::UnitTypes::Protoss_Pylon,bsm.getLastBuiltLocation(base));
                if(pos.isValid()) bsm.isPylonRequired() = true;
            }

            hasBuilt = builder->build(type, pos);
        }

    }

    return hasBuilt;
}

void Tools::DrawUnitCommands()
{
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        const BWAPI::UnitCommand & command = unit->getLastCommand();

        // If the previous command had a ground position target, draw it in red
        // Example: move to location on the map
        if (command.getTargetPosition() != BWAPI::Positions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTargetPosition(), BWAPI::Colors::Red);
        }

        // If the previous command had a tile position target, draw it in red
        // Example: build at given tile position location
        if (command.getTargetTilePosition() != BWAPI::TilePositions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), BWAPI::Position(command.getTargetTilePosition()), BWAPI::Colors::Green);
        }

        // If the previous command had a unit target, draw it in red
        // Example: attack unit, mine mineral, etc
        if (command.getTarget() != nullptr)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTarget()->getPosition(), BWAPI::Colors::White);
        }
    }
}

void Tools::DrawUnitBoundingBoxes()
{
    for (auto& unit : BWAPI::Broodwar->getAllUnits())
    {
        BWAPI::Position topLeft(unit->getLeft(), unit->getTop());
        BWAPI::Position bottomRight(unit->getRight(), unit->getBottom());
        BWAPI::Broodwar->drawBoxMap(topLeft, bottomRight, BWAPI::Colors::White);
    }
}

void Tools::SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target)
{
    // if there's no valid unit, ignore the command
    if (!unit || !target) { return; }

    // Don't issue a 2nd command to the unit on the same frame
    if (unit->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount()) { return; }

    // If we are issuing the same type of command with the same arguments, we can ignore it
    // Issuing multiple identical commands on successive frames can lead to bugs
    if (unit->getLastCommand().getTarget() == target) { return; }
    
    // If there's nothing left to stop us, right click!
    unit->rightClick(target);
}

int Tools::GetTotalSupply(bool inProgress)
{
    // start the calculation by looking at our current completed supplyt
    int totalSupply = BWAPI::Broodwar->self()->supplyTotal();

    // if we don't want to calculate the supply in progress, just return that value
    if (!inProgress) { return totalSupply; }

    // if we do care about supply in progress, check all the currently constructing units if they will add supply
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // ignore units that are fully completed
        if (unit->isCompleted()) { continue; }

        // if they are not completed, then add their supply provided to the total supply
        totalSupply += unit->getType().supplyProvided();
    }

    return totalSupply;
}


bool Tools::checkIfBuildCommandAlreadyIssued(BWAPI::UnitType unitType)
{
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // get the last command given to the unit
        const BWAPI::UnitCommand& command = unit->getLastCommand();

        // if it's not a build command we can ignore it
        if (command.getType() != BWAPI::UnitCommandTypes::Build) { continue; }

        return command.getUnitType() == unitType;

    }

    return false;
}

bool Tools::checkIfBuildCommandAlreadyIssued(BWAPI::UnitType unitType, int builderID)
{
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getID() != builderID) continue;
        // get the last command given to the unit
        const BWAPI::UnitCommand& command = unit->getLastCommand();

        // if it's not a build command we can ignore it
        if (command.getType() != BWAPI::UnitCommandTypes::Build) { continue; }

        return command.getUnitType() == unitType;

    }

    return false;
}

void Tools::DrawUnitHealthBars()
{
    // how far up from the unit to draw the health bar
    int verticalOffset = -10;

    // draw a health bar for each unit on the map
    for (auto& unit : BWAPI::Broodwar->getAllUnits())
    {
        // determine the position and dimensions of the unit
        const BWAPI::Position& pos = unit->getPosition();
        int left = pos.x - unit->getType().dimensionLeft();
        int right = pos.x + unit->getType().dimensionRight();
        int top = pos.y - unit->getType().dimensionUp();
        int bottom = pos.y + unit->getType().dimensionDown();

        // if it's a resource, draw the resources remaining
        if (unit->getType().isResourceContainer() && unit->getInitialResources() > 0)
        {
            double mineralRatio = (double)unit->getResources() / (double)unit->getInitialResources();
            DrawHealthBar(unit, mineralRatio, BWAPI::Colors::Cyan, 0);
        }
        // otherwise if it's a unit, draw the hp 
        else if (unit->getType().maxHitPoints() > 0)
        {
            double hpRatio = (double)unit->getHitPoints() / (double)unit->getType().maxHitPoints();
            BWAPI::Color hpColor = BWAPI::Colors::Green;
            if (hpRatio < 0.66) hpColor = BWAPI::Colors::Orange;
            if (hpRatio < 0.33) hpColor = BWAPI::Colors::Red;
            DrawHealthBar(unit, hpRatio, hpColor, 0);
            
            // if it has shields, draw those too
            if (unit->getType().maxShields() > 0)
            {
                double shieldRatio = (double)unit->getShields() / (double)unit->getType().maxShields();
                DrawHealthBar(unit, shieldRatio, BWAPI::Colors::Blue, -3);
            }
        }
    }
}

void Tools::DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset)
{
    int verticalOffset = -10;
    const BWAPI::Position& pos = unit->getPosition();

    int left = pos.x - unit->getType().dimensionLeft();
    int right = pos.x + unit->getType().dimensionRight();
    int top = pos.y - unit->getType().dimensionUp();
    int bottom = pos.y + unit->getType().dimensionDown();

    int ratioRight = left + (int)((right - left) * ratio);
    int hpTop = top + yOffset + verticalOffset;
    int hpBottom = top + 4 + yOffset + verticalOffset;

    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), color, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

    int ticWidth = 3;

    for (int i(left); i < right - 1; i += ticWidth)
    {
        BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
    }
}