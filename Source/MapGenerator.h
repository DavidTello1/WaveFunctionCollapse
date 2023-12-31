#pragma once

#include "List.h"
#include "DynArray.h"

struct Tile;
struct Cell;
class RandomNumber;

class MapGenerator
{
public:
	MapGenerator(const int width, const int height, const int cellSize);
	MapGenerator(const int width, const int height, const int cellSize, const DynArray<Tile*>& tiles);
	~MapGenerator();

	void GenerateMap();
	void ResetMap();
	void Step();

	void SetTileset(const DynArray<Tile*>& tileset);

	// --- Getters
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetCellSize() const { return cellSize; }
	Cell* GetCell(unsigned int index) { return cells[index]; }
	Cell* GetCell(unsigned int index) const { return cells[index]; }
	Tile* GetTile(unsigned int index) { return tiles[index]; }
	Tile* GetTile(unsigned int index) const { return tiles[index]; }
	Tile* GetTileByID(int tileID) const;
	const DynArray<Tile*>& GetAllTiles() const { return tiles; }
	const List<unsigned int>& GetPresetCells() const { return presetCells; }

	// --- Setters
	void SetWidth(int width) { this->width = width; Resize(); }
	void SetHeight(int height) { this->height = height; Resize(); }
	void SetCellSize(int size) { cellSize = size; }
	void SetSize(int width, int height) { this->width = width; this->height = height; Resize(); }

	// --- Utils
	bool IsFinished() const { return numCollapsed >= cells.size(); }
	void SetCell(unsigned int index, int tileID); // set and propagate cell
	void ResetCell(unsigned int index);
	void PresetCell(unsigned int index, int tileID); // sets a cell to the specified tile but does not propagate
	void ClearPresetCells();

private:
	int HeuristicPick();
	void CollapseCell(unsigned int index);
	void PropagateCell(unsigned int index);
	List<int> PropagateNeighbours(unsigned int index);
	void FirstStep(); // propagate preset cells
	
	// --- Utils
	int FindTile(int tileID) const;
	int CheckNeighbour(int index, int direction);
	void Resize();

private:
	int width = 0;
	int height = 0;
	int cellSize = 0;

	bool isFirstStep = false;
	int numCollapsed = 0;

	DynArray<Cell*> cells;
	DynArray<Tile*> tiles;

	List<unsigned int> presetCells;

	RandomNumber* RNG;
};
