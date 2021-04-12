#include "Tools.h"
#include "MapTools.h"
#include <thread>
#include <mutex>

void ResourceManager::calculateDistances()
{

    int height = BWAPI::Broodwar->mapHeight();
    int width = BWAPI::Broodwar->mapWidth();

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
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

Resource::Resource(int id, size_t x, size_t y, int resourceAmount, bool isGeyser) :
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
