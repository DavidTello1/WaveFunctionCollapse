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
	void DrawMap();

	// --- Utils
	void Step();
	void Reset();
	bool IsFinished() const { return isCollapsed; }

	// --- Debug
	void SetDrawTextures(bool value) { isDrawTextures = value; }
	void SetSpacedCells(bool value) { isSpacedCells = value; }

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

	bool isDrawTextures = true; // for debug purposes
	bool isSpacedCells = false; // for debug purposes

	DynArray<Cell*> cells;
	DynArray<Tile*> tiles; // tiles are created outside but they are owned (and deleted) by this class
};

