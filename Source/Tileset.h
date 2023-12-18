#pragma once
#include "DynArray.h"

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

	void AddTile(Tile* tile);
	void RemoveTile(int index);

	void UpdateTile(int index, const Tile& tile);
	void UpdateMask(int index, int dir, int bit, bool value);

private:
	DynArray<Tile*> tiles;
};
