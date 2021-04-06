
#include "Grid.hpp"
#pragma once

struct Resource
{
    int m_id;
    size_t m_x;
    size_t m_y;
    int m_resourceAmount;
    bool m_isGeyser = false;

    Resource(int id, size_t x, size_t y, int resourceAmount, bool isGeyser);
    Resource()
    {

    }
};


class ResourceManager
{

public:

    Grid<Resource> m_mineralResource;      // mineral resource handle
    Grid<Resource> m_refineryResource;      // refinery resource handle
    bool isJobComplete = false;

    ResourceManager();
    void calculateDistances();
    Resource getMineralResource(int x, int y);
    void setMineralResource(int x, int y, Resource unit);
    Resource getRefineryResource(int x, int y);
    void setRefineryResource(int x, int y, Resource unit);
};
