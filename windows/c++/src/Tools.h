#pragma once

#include <BWAPI.h>
#include "BuildingStrategyManager.h"
#include "ResourceManager.h"

namespace Tools
{

    BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units);
    BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units);
    Resource GetClosestResourceMineralToUnit(BWAPI::Position p);
    std::vector<Resource> GetAllMinerals(BWAPI::Position p);
    std::vector<Resource> GetBaseLocationMineralsList(std::vector<Resource>& allMineralsList);
    bool isResourceInOurList(Resource& resource, std::vector<Resource>& resourceList);
    bool isMineralInOurList(BWAPI::Unit mineral, std::vector<Resource>& resourceList);
    Resource GetClosestGeyserToUnit(BWAPI::Position p);

    int CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units);

    int CountBuildingUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units);

    BWAPI::Unit GetUnitOfType(BWAPI::UnitType type);
    BWAPI::Unit GetBuilderNotBuildingCurrentlyOfType(BWAPI::UnitType type);
    BWAPI::Unit GetTrainerUnitNotFullOfType(BWAPI::UnitType type);
    BWAPI::Unit GetIdleBuilder();
    BWAPI::Unit GetDepot();

    bool BuildBuilding(BWAPI::UnitType type, BuildingStrategyManager& bsm);

    void DrawUnitBoundingBoxes();
    void DrawUnitCommands();

    void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

    int GetTotalSupply(bool inProgress = false);

    void DrawUnitHealthBars();
    void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);
}