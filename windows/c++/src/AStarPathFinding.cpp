#include "AStarPathFinding.h"

double AStarPathFinding::estimateCost(AStarNode n1, AStarNode n2)
{
	const size_t dx = abs(n2.tilePos.x - n1.tilePos.x);
	const size_t dy = abs(n2.tilePos.y - n1.tilePos.y);
	const size_t hDiag = dx >= dy ? dy : dx;
	const size_t hStraight = dx + dy;
	double cost = 141 * hDiag + 100 * (hStraight - 2 * hDiag);

	//std::cout << cost << std::endl;

	return cost;
}

void AStarPathFinding::startSearch(BWAPI::TilePosition& startPos, std::vector<Resource>& vespeneGeyserList, BuildingStrategyManager& bm, Grid<int>& walkable, Grid<int>& buildable)
{
	m_openListGrid = Grid<AStarNode>(walkable.width(), walkable.height(), AStarNode());
	m_startNode = AStarNode(startPos,nullptr,0,0);
	m_goalNode = AStarNode(BWAPI::TilePosition(vespeneGeyserList[0].m_x, vespeneGeyserList[0].m_y), nullptr, DBL_MAX, 0);

	double estimatedCost = estimateCost(m_startNode, m_goalNode);

	m_startNode.hCost = estimatedCost;

	std::vector<Resource> sortedList;

	m_openList.push(m_startNode);

	for (size_t i=0; i<m_openList.size(); i++)
	{
		AStarNode node = m_openList.top();
		m_openList.pop();

		if (node.tilePos == m_goalNode.tilePos) return;

		if (m_closedList[std::to_string(node.tilePos.x) + std::to_string(node.tilePos.y)]) continue;

		for (int d=0; d<8; d++)
		{
			const int x = node.tilePos.x + bm.m_directions[d].x;
			const int y = node.tilePos.y + bm.m_directions[d].y;

			const int cost = m_actionCost[i];

			const BWAPI::TilePosition nodeTile = BWAPI::TilePosition(x, y);
			
			if (nodeTile.isValid() && walkable.get(x,y))
			{
				if (m_openListGrid.get(x,y).gCost != -1 && m_openListGrid.get(x, y).hCost != -1)
				{
					if (m_closedList[std::to_string(x) + std::to_string(y)]) continue;

					if (m_openListGrid.get(x, y).gCost <= cost)
					{
						continue;
					}

					AStarNode childNode = AStarNode(nodeTile, &node, cost, estimatedCost);
					estimatedCost = estimateCost(childNode,m_goalNode);
					childNode.hCost = estimatedCost;

					m_openList.push(childNode);
				}
			}

		}

	}


}
