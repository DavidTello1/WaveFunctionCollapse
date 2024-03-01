#pragma once
#include "Point2D.h"

#include "DynArray.h"
//#include "List.h"

#include <list> //***

// ----------------------------------
struct PathNode
{
public:
	PathNode() : g(0), h(0), cost(1), pos(0, 0), parent(nullptr) {};
	PathNode(int g, int h, int cost, const Point2D pos, PathNode* parent) : g(g), h(h), cost(cost), pos(pos), parent(parent) {};

	int GetScore() const { return g + h; }

	int CalculateF(const Point2D& destination) {
		g = parent->g + cost;
		h = pos.DistanceTo(destination);

		return g + h;
	}

public:
	int g = 0;
	int h = 0;
	int cost = 1;
	Point2D pos = { 0, 0 };
	PathNode* parent = nullptr; // needed to reconstruct the path in the end
};


// ----------------------------------
class Pathfinding
{
public:
	Pathfinding(int width, int height, const DynArray<int>& costMap) : mapWidth(width), mapHeight(height), costMap(costMap) {};
	~Pathfinding() {};

	const DynArray<int>& GetPath() const { return path; }

	int Propagate(const Point2D& origin, const Point2D& destination);

private:
	// --- Utils
	std::list<PathNode>::iterator FindNode(std::list<PathNode>& list, const Point2D& pos) const;
	std::list<PathNode>::const_iterator GetLowestScoreNode(const std::list<PathNode>& list) const;
	void GetValidNeighbours(PathNode& node, std::list<PathNode>& neighbours); // fills list of neighbours

private:
	int mapWidth = 0;
	int mapHeight = 0;
	const DynArray<int> costMap;

	DynArray<int> path;
};
