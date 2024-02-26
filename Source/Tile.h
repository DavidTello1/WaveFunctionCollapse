#pragma once
#include "StaticArray.h"
#include "BitArray.h"

#define NUM_NEIGHBOURS 4

struct Tile 
{
	friend class Tileset;

public:
	Tile(const int id, unsigned int texture, bool isWalkable, const StaticArray<BitArray, NUM_NEIGHBOURS> masks) : id(id), texture(texture), isWalkable(isWalkable), masks(masks) {};

	Tile(const int id, unsigned int texture, bool isWalkable, const char* top, const char* left, const char* right,const char* bottom) : id(id), texture(texture), isWalkable(isWalkable) 
	{
		masks[0] = BitArray(top);
		masks[1] = BitArray(left);
		masks[2] = BitArray(right);
		masks[3] = BitArray(bottom);
	}

	Tile(const int id, unsigned int texture, bool isWalkable, const unsigned int maskSize) : id(id), texture(texture), isWalkable(isWalkable)
	{
		masks[0] = BitArray(maskSize);
		masks[1] = BitArray(maskSize);
		masks[2] = BitArray(maskSize);
		masks[3] = BitArray(maskSize);
	}

	Tile(Tile* tile) : id(tile->id), texture(tile->texture), isWalkable(tile->isWalkable), masks(tile->masks) {};

	~Tile() {}

	int GetID() const { return id; }
	unsigned int GetTexture() const { return texture; }
	bool IsWalkable() const { return isWalkable; }
	StaticArray<BitArray, NUM_NEIGHBOURS> GetMasks() const { return masks; }

	void SetMask(int dir, BitArray mask) { masks[dir] = mask; }
	void SetWalkable(bool value) { isWalkable = value; }

protected:
	int id = -1;
	unsigned int texture = 0;
	bool isWalkable = true;
	StaticArray<BitArray, NUM_NEIGHBOURS> masks; // top, left, right, bottom
};
