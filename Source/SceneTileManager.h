#pragma once
#include "DynArray.h"

struct Tile;

class SceneTileManager
{
	friend class SceneTileManagerUI;

public: 
	SceneTileManager();
	virtual ~SceneTileManager();

	bool Init();
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	// --- Utils
	Tile* GetTile(int index) { return tiles[index]; }
	DynArray<Tile*> GetTiles() { return tiles; }

private:
	DynArray<Tile*> tiles;
};

