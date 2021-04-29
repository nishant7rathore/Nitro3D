#pragma once

#include <BWAPI.h>
#include <vector>
#include "Tools.h"
#include "ResourceManager.h"


struct MapTools
{
    Grid<int>   m_walkable;       // whether a tile is buildable (includes static resources)          
    Grid<int>   m_buildable;      // whether a tile is buildable (includes static resources)
    Grid<int>   m_depotBuildable; // whether a depot is buildable on a tile (illegal within 3 tiles of static resource)
    Grid<int>   m_lastSeen;       // the last time any of our units has seen this position on the map
    std::vector<BWAPI::TilePosition> m_baseLocations;
    std::vector<Resource> m_allMinerals;
    int         m_width = 0;
    int         m_height = 0;
    int         m_frame = 0;
    bool        m_drawMap = false;

    bool canBuild(int tileX, int tileY) const;
    bool canWalk(int tileX, int tileY) const;
    void printMap() const;

    MapTools() noexcept;

    void onStart(ResourceManager& rm, BuildingStrategyManager& bm);
    void    onFrame();
    void    draw() const;
    void    toggleDraw();

    int     width() const;
    int     height() const;

    bool    isValidTile(int tileX, int tileY) const;
    bool    isValidTile(const BWAPI::TilePosition & tile) const;
    bool    isValidPosition(const BWAPI::Position & pos) const;
    bool    isPowered(int tileX, int tileY) const;
    bool    isExplored(int tileX, int tileY) const;
    bool    isExplored(const BWAPI::Position & pos) const;
    bool    isExplored(const BWAPI::TilePosition & pos) const;
    bool    isVisible(int tileX, int tileY) const;
    bool    isWalkable(int tileX, int tileY) const;
    bool    isWalkable(const BWAPI::TilePosition& tile) const;
    bool    isBuildable(int tileX, int tileY) const;
    bool    isBuildable(const BWAPI::TilePosition& tile) const;
    bool    isDepotBuildableTile(int tileX, int tileY) const;
    void    drawTile(int tileX, int tileY, const BWAPI::Color & color) const;

};