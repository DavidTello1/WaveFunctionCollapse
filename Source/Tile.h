#pragma once
#include "StaticArray.h"
#include "BitArray.h"

#define NUM_NEIGHBOURS 8

struct Tile 
{
public:
	Tile(const int id, unsigned int texture, const StaticArray<BitArray, NUM_NEIGHBOURS> masks) : id(id), texture(texture), masks(masks) {};

	Tile(const int id, unsigned int texture, 
		const char* topLeft, const char* top, const char* topRight,
		const char* left, const char* right, 
		const char* bottomLeft, const char* bottom, const char* bottomRight) : id(id), texture(texture) 
	{
		masks[0] = BitArray(topLeft);
		masks[1] = BitArray(top);
		masks[2] = BitArray(topRight);
		masks[3] = BitArray(left);
		masks[4] = BitArray(right);
		masks[5] = BitArray(bottomLeft);
		masks[6] = BitArray(bottom);
		masks[7] = BitArray(bottomRight);
	}

	~Tile() {
	}

public://***SHOULD BE PRIVATE, READ-ONLY CLASS
	int id = -1;
	unsigned int texture = 0;
	StaticArray<BitArray, NUM_NEIGHBOURS> masks; // topLeft, top, topRight, left, right, bottomLeft, bottom, bottomRight;
};
