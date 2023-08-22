#pragma once

#include "List.h"
#include "DynArray.h"

struct Tile;
struct Cell;

class MapGenerator
{
public:
	MapGenerator(const int width, const int height, const int cellSize, const DynArray<Tile*>& tiles);
	~MapGenerator();

	void GenerateMap();
	void Step();
	void Reset();

	//void PresetCell(unsigned int index, unsigned int tileID);
	//void PresetCells(List<unsigned int> cells, List<unsigned int> tileIDs);
	//void ClearPresetCells();

	// --- Utils
	bool IsFinished() const { return isCollapsed; }
	Cell* GetCell(unsigned int index) { return cells[index]; }
	Cell* GetCell(unsigned int index) const { return cells[index]; }
	Tile* GetTile(unsigned int index) { return tiles[index]; }
	Tile* GetTile(unsigned int index) const { return tiles[index]; }

private:
	int HeuristicPick();
	void CollapseCells();
	List<int> PropagateCell(const int index);
	
	// --- Utils
	int CheckNeighbour(int index, int direction);

private:
	int width = 0;
	int height = 0;
	int cellSize = 0;

	bool isCollapsed = false;

	DynArray<Cell*> cells;
	DynArray<Tile*> tiles; // tiles are created outside but they are owned (and deleted) by this class

	DynArray<unsigned int> presetCells;
};

