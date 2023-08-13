#pragma once
#include "StaticArray.h"
#include "BitMask.h"

struct Tile 
{
public:
	int id = -1;
	StaticArray<BitMask*, 4> masks; // top, left, right, bottom

public:
	Tile(const int id, const StaticArray<BitMask*, 4> masks) : id(id), masks(masks) {};

	Tile(const int id, const char* top, const char* left, const char* right, const char* bottom) : id(id) {
		masks[0] = new BitMask(top);
		masks[1] = new BitMask(left);
		masks[2] = new BitMask(right);
		masks[3] = new BitMask(bottom);
	}

	~Tile() {
		for (int i = 0; i < 4; ++i) 
		{
			delete masks[i];
			masks[i] = nullptr;
		}
	}
};
