#pragma once
#include "DynArray.h"
#include "BitArray.h"

struct Tile;

class Tileset
{
public:
	Tileset();
	~Tileset();

	unsigned int GetSize() const { return tiles.size(); }

	Tile* GetTile(int index) { return tiles[index]; }
	const Tile* GetTile(int index) const { return tiles[index]; }
	const DynArray<Tile*>& GetAllTiles() const { return tiles; }

	bool IsValid(int tileID);

	void AddTile(int tileID, unsigned int texture);
	void AddTile(const Tile& tile); //adds a tile without resizing masks
	void RemoveTile(int index);
	void Clear();

	void SetTile(int index, const Tile& tile);
	void SetMask(int index, int dir, const BitArray& mask);
	void UpdateMask(int index, int dir, int bit, bool value);
	void SetWalkable(int index, bool value);
	void SetSymmetry(int index, bool value);
	void SetCost(int index, int value);

private:
	DynArray<Tile*> tiles;
};
