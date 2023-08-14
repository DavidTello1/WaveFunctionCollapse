#pragma once
#include "StaticArray.h"
#include "BitMask.h"

struct SDL_Texture;

struct Tile 
{
public:
	int id = -1;
	SDL_Texture* texture = nullptr;
	StaticArray<BitMask*, 4> masks; // top, left, right, bottom

public:
	Tile(const int id, SDL_Texture* texture, const StaticArray<BitMask*, 4> masks) : id(id), texture(texture), masks(masks) {};

	Tile(const int id, SDL_Texture* texture, const char* top, const char* left, const char* right, const char* bottom) : id(id), texture(texture) {
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
