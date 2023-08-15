#pragma once
#include "StaticArray.h"
#include "BitMask.h"

#define NUM_NEIGHBOURS 8

struct SDL_Texture;

struct Tile 
{
public:
	int id = -1;
	SDL_Texture* texture = nullptr;
	StaticArray<BitMask*, NUM_NEIGHBOURS> masks; // topLeft, top, topRight, left, right, bottomLeft, bottom, bottomRight;

public:
	Tile(const int id, SDL_Texture* texture, const StaticArray<BitMask*, NUM_NEIGHBOURS> masks) : id(id), texture(texture), masks(masks) {};

	Tile(const int id, SDL_Texture* texture, 
		const char* topLeft, const char* top, const char* topRight,
		const char* left, const char* right, 
		const char* bottomLeft, const char* bottom, const char* bottomRight) : id(id), texture(texture) 
	{
		masks[0] = new BitMask(topLeft);
		masks[1] = new BitMask(top);
		masks[2] = new BitMask(topRight);
		masks[3] = new BitMask(left);
		masks[4] = new BitMask(right);
		masks[5] = new BitMask(bottomLeft);
		masks[6] = new BitMask(bottom);
		masks[7] = new BitMask(bottomRight);
	}

	~Tile() {
		for (int i = 0; i < NUM_NEIGHBOURS; ++i)
		{
			delete masks[i];
			masks[i] = nullptr;
		}
	}
};
