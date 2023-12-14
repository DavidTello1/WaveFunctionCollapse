#pragma once
#include "StaticArray.h"
#include "DynArray.h"
#include "BitArray.h"

#define NUM_NEIGHBOURS 4

struct Tile 
{
	friend class Tileset;

public:
	Tile(const int id, unsigned int texture, const StaticArray<BitArray, NUM_NEIGHBOURS> masks) : id(id), texture(texture), masks(masks) {};

	Tile(const int id, unsigned int texture, const char* top, const char* left, const char* right,const char* bottom) : id(id), texture(texture) 
	{
		masks[0] = BitArray(top);
		masks[1] = BitArray(left);
		masks[2] = BitArray(right);
		masks[3] = BitArray(bottom);
	}

	~Tile() {}

	int GetID() const { return id; }
	unsigned int GetTexture() const { return texture; }
	StaticArray<BitArray, NUM_NEIGHBOURS> GetMasks() const { return masks; }

protected:
	int id = -1;
	unsigned int texture = 0;
	StaticArray<BitArray, NUM_NEIGHBOURS> masks; // top, left, right, bottom
};

// -------------------------------
class Tileset
{
private:
	DynArray<Tile*> tiles;


public:
	Tileset() 
	{
	}

	~Tileset() 
	{
		for (unsigned int i = 0; i < tiles.size(); ++i)
			delete tiles[i];
		tiles.clear();
	}

	Tile* GetTile(int index)
	{
		return tiles[index];
	}

	const Tile* GetTile(int index) const 
	{ 
		return tiles[index];
	}

	const unsigned int GetSize() const 
	{
		return tiles.size(); 
	}

	void AddTile(Tile* tile)
	{
		tiles.push_back(tile);
	}

	void RemoveTile(int index)
	{
		//tiles.erase(index);
	}

	void UpdateTile(int index, const Tile& tile)
	{
		*tiles[index] = tile;
	}

	void UpdateMask(int index, int dir, int bit, bool value)
	{
		if (value)
			tiles[index]->masks[dir].setBit(bit);
		else
			tiles[index]->masks[dir].clearBit(bit);
	}
};
