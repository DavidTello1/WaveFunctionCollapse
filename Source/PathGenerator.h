#pragma once
#include "DynArray.h"
#include <map>

#define NUM_NEIGHBOURS 4
#define MIN_AREA_SIZE 5

class MapGenerator;

struct Area {
	int label;

	DynArray<int> cells; // cellIndices contained in this area
	DynArray<int> neighbourAreas; // labels of neighbouring areas
};

class PathGenerator
{
public:
	PathGenerator(MapGenerator* map);
	~PathGenerator();

	DynArray<int> GeneratePaths();
	void Reset();

	const std::map<int, DynArray<int>> GetAreas() const { return areas; }
	const std::map<int, int>& GetBreadcrumbs() const { return breadCrumbs; }
	const DynArray<DynArray<int>>& GetPaths() const { return paths; }

private:
	void FindAreas(); // Connected-Component-Labeling
	void CreateAreas(); // Fill areas map
	void RemoveAreas(int minSize); // Remove areas smaller than minSize
	void GetConnections(); // Get connections to neighbouring areas
	void SetBreadCrumbs(); // Select random point in each area
	void CarvePaths(); // Pathfinding to connect areas
	void FinishGeneration(); // Final map tiles (WFC of reset cells)

private:
	MapGenerator* map = nullptr; // reference to MapGenerator (shared_ptr)

	std::map<int, DynArray<int>> areas;

	DynArray<bool> walkabilityMap;
	DynArray<int> labelingMap;
	DynArray<int> costMap;

	std::map<int, DynArray<int>> connections;
	std::map<int, int> breadCrumbs;

	DynArray<DynArray<int>> paths;
};


// Get Areas (Connected Component Labeling)
// Set 1 Point per Area
// Connect Points (Delaunay Triangulation - BowyerWatson)
// Select Paths (Prim's Algorithm)
// Carve Paths (Dijkstra)
// Set non-walkable Tiles in path to walkable tile and reset neighbours
// WFC of reset cells