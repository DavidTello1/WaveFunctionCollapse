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
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	Cell* GetCell(unsigned int index) { return cells[index]; }
	Cell* GetCell(unsigned int index) const { return cells[index]; }
	Tile* GetTile(unsigned int index) { return tiles[index]; }
	Tile* GetTile(unsigned int index) const { return tiles[index]; }

	// --- Setters
	void SetWidth(int width) { this->width = width; Resize(); }
	void SetHeight(int height) { this->height = height; Resize(); }
	void SetSize(int width, int height) { this->width = width; this->height = height; Resize(); }

	// --- Utils
	bool IsFinished() const { return isCollapsed; }
	void SetCell(unsigned int index, unsigned int tileID); // set and propagate cell
	void ResetCell(unsigned int index);
	void PresetCell(unsigned int index, unsigned int tileID); // sets a cell to the specified tile but does not propagate
	void ClearPresetCells();

private:
	int HeuristicPick();
	void CollapseCell();
	List<int> PropagateCell(const int index);
	void FirstStep(); // propagate preset cells
	
	// --- Utils
	int CheckNeighbour(int index, int direction);
	void Resize();

private:
	int width = 0;
	int height = 0;
	int cellSize = 0;

	bool isCollapsed = false;
	bool isFirstStep = false;

	DynArray<Cell*> cells;
	DynArray<Tile*> tiles; // tiles are created outside but they are owned (and deleted) by this class

	List<unsigned int> presetCells;
};
