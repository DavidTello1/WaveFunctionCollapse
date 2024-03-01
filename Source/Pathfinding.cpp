#include "Pathfinding.h"

#include "Globals.h"

int Pathfinding::Propagate(const Point2D& origin, const Point2D& destination)
{
	int ret = -1;

	std::list<PathNode> open;
	std::list<PathNode> close;

	open.push_back(PathNode(0, 0, 0, origin, nullptr));

	while (!open.empty()) 
	{

		std::list<PathNode>::const_iterator aux = GetLowestScoreNode(open);
		close.push_back(*aux);

		std::list<PathNode>::iterator lower = std::prev(close.end());
		open.erase(aux);

		if ((*lower).pos == destination) 
		{
			path.clear();
			const PathNode* new_node = &(*lower);

			while (new_node) 
			{
				int cell = new_node->pos.x + new_node->pos.y * mapWidth;

				path.push_back(cell);
				new_node = new_node->parent;
			}

			path = path.flip();
			return path.size();
		}

		std::list<PathNode> neighbours;
		GetValidNeighbours(*lower, neighbours);

		for (std::list<PathNode>::iterator it = neighbours.begin(); it != neighbours.end(); it = std::next(it)) 
		{
			if (FindNode(close, (*it).pos) != close.end())
				continue;

			std::list<PathNode>::iterator neighbour = FindNode(open, (*it).pos);

			if (neighbour == open.end()) {

				(*it).CalculateF(destination);
				open.push_back(*it);
			}
			else if ((*neighbour).g > (*it).g + 1) {

				(*neighbour).parent = (*it).parent;
				(*neighbour).CalculateF(destination);
			}
		}
	}

	return 0;
}

std::list<PathNode>::iterator Pathfinding::FindNode(std::list<PathNode>& list, const Point2D& pos) const
{
	std::list<PathNode>::iterator item = list.begin();

	while (item != list.end())
	{
		if ((*item).pos == pos)
			return item;

		item = next(item);
	}

	return list.end();
}

std::list<PathNode>::const_iterator Pathfinding::GetLowestScoreNode(const std::list<PathNode>& list) const
{
	std::list<PathNode>::const_iterator ret;
	int min = 65535;

	std::list<PathNode>::const_iterator item = list.begin();

	while (item != list.end())
	{
		if ((*item).GetScore() < min)
		{
			min = (*item).GetScore();
			ret = item;
		}
		item = next(item);
	}

	return ret;
}

void Pathfinding::GetValidNeighbours(PathNode& node, std::list<PathNode>& neighbours)
{
	// north
	if (node.pos.y >= 1)
	{
		Point2D coords = Point2D(node.pos.x, node.pos.y - 1);
		int index = coords.x + coords.y * mapWidth;
		int cost = costMap[index];

		neighbours.push_back(PathNode(-1, -1, cost, coords, &node));
	}

	// south
	if (node.pos.y < mapHeight - 1)
	{
		Point2D coords = Point2D(node.pos.x, node.pos.y + 1);
		int index = coords.x + coords.y * mapWidth;
		int cost = costMap[index];

		neighbours.push_back(PathNode(-1, -1, cost, coords, &node));
	}

	// east
	if (node.pos.x < mapWidth - 1)
	{
		Point2D coords = Point2D(node.pos.x + 1, node.pos.y);
		int index = coords.x + coords.y * mapWidth;
		int cost = costMap[index];

		neighbours.push_back(PathNode(-1, -1, cost, coords, &node));
	}

	// west
	if (node.pos.x >= 1)
	{
		Point2D coords = Point2D(node.pos.x - 1, node.pos.y);
		int index = coords.x + coords.y * mapWidth;
		int cost = costMap[index];

		neighbours.push_back(PathNode(-1, -1, cost, coords, &node));
	}
}
