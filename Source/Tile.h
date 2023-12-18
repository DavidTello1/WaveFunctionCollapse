#pragma once
#include "StaticArray.h"
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

	Tile(Tile* tile) : id(tile->id), texture(tile->texture), masks(tile->masks) {};

	~Tile() {}

	int GetID() const { return id; }
	unsigned int GetTexture() const { return texture; }
	StaticArray<BitArray, NUM_NEIGHBOURS> GetMasks() const { return masks; }

protected:
	int id = -1;
	unsigned int texture = 0;
	StaticArray<BitArray, NUM_NEIGHBOURS> masks; // top, left, right, bottom
};
