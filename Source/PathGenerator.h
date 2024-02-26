#pragma once
#include "DynArray.h"
#include <map>

class MapGenerator;

class PathGenerator
{
public:
	PathGenerator(MapGenerator* map);
	~PathGenerator();

	DynArray<int> GeneratePaths();
	void Reset();

	const std::map<int, DynArray<int>> GetAreas() const { return areas; }
	const DynArray<int>& GetBreadcrumbs() const { return breadCrumbs; }

private:
	void GetAreas_FirstPass(); // Connected-Component-Labeling (First Pass)
	void GetAreas_SecondPass(); // Connected-Component-Labeling (Second Pass)

	void ConnectAreas(); // Bowyer-Watson
	void GetPaths(); // Prim's algorithm
	void CarvePaths(); // Dijkstra
	void FinishGeneration(); // Final map tiles (WFC of reset cells)

private:
	static constexpr int MIN_AREA_SIZE = 5;

	MapGenerator* map = nullptr; // reference to MapGenerator (shared_ptr)

	std::map<int, DynArray<int>> areas;
	DynArray<int> walkableCells;

	DynArray<int> labelingMap;
	std::map<int, int> equivalencies;

	DynArray<int> breadCrumbs;
};


// Get Areas (Connected Component Labeling)
// Set 1 Point per Area
// Connect Points (Delaunay Triangulation - BowyerWatson)
// Select Paths (Prim's Algorithm)
// Carve Paths (Dijkstra)
// Set non-walkable Tiles in path to walkable tile and reset neighbours
// WFC of reset cells