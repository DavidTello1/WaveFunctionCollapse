#pragma once
#include "Scene.h"

#include "DynArray.h"

struct Tile;

struct TileData {
	const char* name;
	const char* filepath;
	int tileIndex;

	bool isSelected;
	bool isChanged;
	
	bool isTopSet;
	bool isLeftSet;
	bool isRightSet;
	bool isBottomSet;
};

class SceneTileManager : public Scene
{
public: 
	SceneTileManager();
	virtual ~SceneTileManager();

	bool Init() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	bool DrawUI() override;

private:
	//// --- File Manager
	//bool Import();
	//bool Export();

	// --- UI Panels
	void DrawMenuBar();
	void DrawToolbar();
	void DrawHierarchy();
	void DrawMainPanel();
	void DrawNeighbours();
	void DrawTileInfo();
	void DrawTileList();

private:
	int currentTile = 0;
	int currentDir = 0;
	DynArray<Tile*> tiles;
	DynArray<TileData*> tileData;

	bool isFilter = false;
};

