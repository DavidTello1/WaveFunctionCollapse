#pragma once
#include "DynArray.h"
#include "BitArray.h"
#include <map>

#define NUM_NEIGHBOURS 4
#define MIN_AREA_SIZE 5

class MapGenerator;
struct Tile;

class PathGenerator
{
public:
	PathGenerator(MapGenerator* map);
	~PathGenerator();

	DynArray<int> GeneratePaths();
	void Step();
	void Reset();

	void SetTileset(const DynArray<Tile*>& tileset);

	const std::map<int, DynArray<int>> GetAreas() const { return areas; }
	const std::map<int, int>& GetBreadcrumbs() const { return breadCrumbs; }
	const DynArray<DynArray<int>>& GetPaths() const { return paths; }
	const DynArray<bool>& GetWalkabilityMap() const { return walkabilityMap; }

private:
	void Init();
	void FindAreas(); // Connected-Component-Labeling
	void CreateAreas(); // Fill areas map
	void RemoveAreas(int minSize); // Remove areas smaller than minSize
	void GetConnections(); // Get connections to neighbouring areas
	void SetBreadCrumbs(); // Select random point in each area
	void CalcPaths(); // Pathfinding to connect areas
	void CarvePaths(); // Remove walls in paths and its neighbours
	void LastChecks(); // Remove disconnected areas
	void ResetNeighbours(); // Reset all neighbours of walkable cells
	void FinishGeneration(); // Final map tiles (WFC of reset cells)

	// --- Utils
	void ChangeTileset(bool expanded);
	int CheckNeighbour(int index, int direction) const;
	int FindRoot(const int label, std::map<int, int>& equivalencies) const;

private:
	MapGenerator* map = nullptr; // reference to MapGenerator (shared_ptr)

	std::map<int, DynArray<int>> areas;

	DynArray<bool> walkabilityMap;
	DynArray<int> labelingMap;
	DynArray<int> costMap;

	std::map<int, DynArray<int>> connections;
	std::map<int, int> breadCrumbs;

	DynArray<DynArray<int>> paths;

	BitArray nonWalkableMask;

	// ---
	int step = 0;

	DynArray<Tile*> tiles; //copy of tileset in WFC, owned by WFC
	DynArray<Tile*> tiles_expanded; // tileset with more tiles to fill gaps in FinishGeneration()
};
