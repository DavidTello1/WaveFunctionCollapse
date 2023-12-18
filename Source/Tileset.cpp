#include "Tileset.h"
#include "Tile.h"

#include "mmgr/mmgr.h"

Tileset::Tileset()
{
}

Tileset::~Tileset()
{
	for (unsigned int i = 0; i < tiles.size(); ++i)
		delete tiles[i];
	tiles.clear();
}

void Tileset::AddTile(Tile* tile) {
	tiles.push_back(tile);
}

void Tileset::RemoveTile(int index) {
	//tiles.erase(index); //***
}

void Tileset::UpdateTile(int index, const Tile& tile) {
	*tiles[index] = tile;
}

void Tileset::UpdateMask(int index, int dir, int bit, bool value) {
	if (value)
		tiles[index]->masks[dir].setBit(bit);
	else
		tiles[index]->masks[dir].clearBit(bit);
}

bool Tileset::IsValid(int tileID) {
	for (unsigned int i = 0; i < tiles.size(); ++i)
	{
		if (tiles[i]->id == tileID)
			return true;
	}
	return false;
}
