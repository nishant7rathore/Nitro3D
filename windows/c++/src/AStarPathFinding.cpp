#include "AStarPathFinding.h"

double AStar::estimateCost(AStarNode n1, AStarNode n2)
{
	const size_t dx = abs(n2.tilePos.x - n1.tilePos.x);
	const size_t dy = abs(n2.tilePos.y - n1.tilePos.y);
	const size_t hDiag = dx >= dy ? dy : dx;
	const size_t hStraight = dx + dy;
	double cost = 141 * hDiag + 100 * (hStraight - 2 * hDiag);

	//std::cout << cost << std::endl;

	return cost;
}

void AStar::startSearch(BWAPI::TilePosition& startPos, std::vector<Resource>& vespeneGeyserList, BuildingStrategyManager& bm, Grid<int>& walkable, Grid<int>& buildable)
{
	
}
