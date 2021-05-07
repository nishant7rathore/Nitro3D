#include "AStarPathFinding.h"

AStarPathFinding::AStarPathFinding(std::vector<Resource>& resources)
{
	m_baseLocations.m_allMinerals = resources;
}

double AStarPathFinding::estimateCost(AStarNode n1, AStarNode n2)
{
	//const size_t dx = abs(n2.walkPos.x - n1.walkPos.x);
	//const size_t dy = abs(n2.walkPos.y - n1.walkPos.y);
	//const size_t hDiag = dx >= dy ? dy : dx;
	//const size_t hStraight = dx + dy;
	//const double cost = 141 * hDiag + 100 * (hStraight - 2 * hDiag);

	const double newCost = (sqrt(pow(abs(n2.walkPos.x - n1.walkPos.x),2) + pow(abs(n2.walkPos.y - n1.walkPos.y), 2)))*8;

	return newCost;
}

int AStarPathFinding::startSearch(BWAPI::WalkPosition& startPos, BWAPI::WalkPosition& goalPos, BuildingStrategyManager& bm, Grid<int>& walkable, Grid<int>& buildable)
{
	m_openList = std::priority_queue<AStarNode, std::vector<AStarNode>, NodeCostComparion>();
	m_openListGrid = Grid<AStarNode>(4*walkable.width(), 4*walkable.height(), AStarNode());
	m_closedList = Grid<int>(4*walkable.width(), 4*walkable.height(), -1);

	m_startNode = AStarNode(startPos,nullptr,0,0);
	m_goalNode = AStarNode(BWAPI::WalkPosition(goalPos.x, goalPos.y), nullptr, DBL_MAX, 0); // create start and goal node
	double estimatedCost = estimateCost(m_startNode, m_goalNode); // estimate cost to the goal from the start node
	m_startNode.hCost = estimatedCost;
	m_openList.push(m_startNode);

	while (m_openList.size())
	{
		AStarNode node = m_openList.top();
		m_openList.pop();

		if (node.walkPos == m_goalNode.walkPos)
		{
			return node.gCost;
		}

		if (m_closedList.get(node.walkPos.x,node.walkPos.y) > 0)
		{
			continue;
		}

		m_closedList.set(node.walkPos.x, node.walkPos.y, (int)m_startNode.hCost);

		for (int d=0; d<4; d++)
		{
			const int x = node.walkPos.x + bm.m_directions[d].x;
			const int y = node.walkPos.y + bm.m_directions[d].y;

			if (x < 0 || y < 0 || m_closedList.get(x, y) > 0)
			{
				continue;
			}
			
			const BWAPI::WalkPosition nodeTile = BWAPI::WalkPosition(x, y);
			if (!nodeTile.isValid()) continue;

			const double cost = m_actionCost[d];
			const double totalNodeGCost = cost + node.gCost;

			if (BWAPI::Broodwar->isWalkable(x,y)) // only add the tile if it's walkable
			{

				double oldGCost = m_openListGrid.get(x, y).gCost;
				double oldHCost = m_openListGrid.get(x, y).hCost;

				if (oldGCost <= totalNodeGCost) // done for optimization 
				{
					continue;
				}

				AStarNode childNode = AStarNode(nodeTile, &node, totalNodeGCost, estimatedCost);
				estimatedCost = estimateCost(childNode, m_goalNode);
				childNode.hCost = estimatedCost;
				m_openListGrid.set(x,y,childNode);
				m_openList.push(childNode);
			}

		}

	}

	return INT_MAX;
}

