#pragma once
#include "DynArray.h"
#include "BitArray.h"
#include <map>

#define NUM_NEIGHBOURS 4
#define MIN_AREA_SIZE 5

class MapGenerator;

class PathGenerator
{
public:
	PathGenerator(MapGenerator* map);
	~PathGenerator();

	DynArray<int> GeneratePaths();
	void Step();
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
	void CalcPaths(); // Pathfinding to connect areas
	void CarvePaths(); // Remove walls in paths and its neighbours
	void FinishGeneration(); // Final map tiles (WFC of reset cells)

	// --- Utils
	void CreateWalkableMask();
	void ResetNeighbours(int index);

private:
	MapGenerator* map = nullptr; // reference to MapGenerator (shared_ptr)

	std::map<int, DynArray<int>> areas;

	DynArray<bool> walkabilityMap;
	DynArray<int> labelingMap;
	DynArray<int> costMap;

	std::map<int, DynArray<int>> connections;
	std::map<int, int> breadCrumbs;

	DynArray<DynArray<int>> paths;

	BitArray walkableMask;

	// ---
	int step = 0;
};
