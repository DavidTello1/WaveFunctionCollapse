#include "Tileset.h"
#include "Tile.h"

#include "mmgr/mmgr.h"

Tileset::Tileset()
{
}

Tileset::~Tileset()
{
	Clear();
}

void Tileset::AddTile(int tileID, unsigned int texture)
{
	// Update Masks
	for (unsigned int i = 0; i < tiles.size(); ++i)
	{
		for (unsigned int dir = 0; dir < NUM_NEIGHBOURS; ++dir)
		{
			BitArray mask = tiles[i]->GetMasks()[dir];
			mask.addBit();

			tiles[i]->SetMask(dir, mask);
		}
	}

	Tile* tile = new Tile(tileID, texture, tiles.size() + 1);
	tiles.push_back(tile);
}

void Tileset::AddTile(const Tile& tile)
{
	tiles.push_back(new Tile(tile));
}

void Tileset::RemoveTile(int index)
{
	if (index < 0 || index >= tiles.size())
		return;

	delete tiles[index];
	tiles.erase(index);

	// Update Masks
	for (unsigned int i = 0; i < tiles.size(); ++i)
	{
		for (unsigned int dir = 0; dir < NUM_NEIGHBOURS; ++dir)
		{
			BitArray mask = tiles[i]->GetMasks()[dir];
			mask.eraseBit(index);

			tiles[i]->SetMask(dir, mask);
		}
	}
}

void Tileset::Clear()
{
	for (unsigned int i = 0; i < tiles.size(); ++i)
		delete tiles[i];
	tiles.clear();
}

void Tileset::SetTile(int index, const Tile& tile)
{
	if (index < 0 || index >= tiles.size())
		return;
		
	*tiles[index] = tile;
}

void Tileset::SetMask(int index, int dir, const BitArray& mask)
{
	if (index < 0 || index >= tiles.size())
		return;

	tiles[index]->masks[dir] = mask;
}

void Tileset::UpdateMask(int index, int dir, int bit, bool value)
{
	if (index < 0 || index >= tiles.size())
		return;

	if (value)
		tiles[index]->masks[dir].setBit(bit);
	else
		tiles[index]->masks[dir].clearBit(bit);
}

bool Tileset::IsValid(int tileID)
{
	for (unsigned int i = 0; i < tiles.size(); ++i)
	{
		if (tiles[i]->id == tileID)
			return true;
	}
	return false;
}
