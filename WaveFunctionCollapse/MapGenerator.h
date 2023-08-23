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
	void ResetMap();
	void Step();

	// --- Getters
	Cell* GetCell(unsigned int index) { return cells[index]; }
	Cell* GetCell(unsigned int index) const { return cells[index]; }
	Tile* GetTile(unsigned int index) { return tiles[index]; }
	Tile* GetTile(unsigned int index) const { return tiles[index]; }

	// --- Utils
	bool IsFinished() const { return isCollapsed; }
	void SetCell(unsigned int index, unsigned int tileID); // set and propagate cell
	void ResetCell(unsigned int index);
	void PresetCell(unsigned int index, unsigned int tileID); // Sets a cell to the defined tile but does not propagate
	void ClearPresetCells();

private:
	int HeuristicPick();
	void CollapseCell();
	List<int> PropagateCell(const int index);
	void FirstStep(); // propagate preset cells
	
	// --- Utils
	int CheckNeighbour(int index, int direction);

private:
	int width = 0;
	int height = 0;
	int cellSize = 0;

	bool isCollapsed = false;

	DynArray<Cell*> cells;
	DynArray<Tile*> tiles; // tiles are created outside but they are owned (and deleted) by this class

	List<unsigned int> presetCells;
	bool isFirstStep = false;
};

