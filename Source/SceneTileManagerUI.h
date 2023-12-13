#pragma once
#include "DynArray.h"
#include "BitArray.h"
#include "String.h"

class SceneTileManager;

// --- Events
struct EventImportTile;
struct EventImportTileset;
struct EventImportMap;
struct EventExportTileset;
struct EventExportMap;

// ---

struct TileData {
	String name;
	String filepath;
	int tileID;
	unsigned int texture;

	bool isSelected;
	bool isChanged;
};

class SceneTileManagerUI
{
public:
	SceneTileManagerUI(SceneTileManager* sceneTiles);
	~SceneTileManagerUI();

	bool Init();
	bool Start();
	bool CleanUp();

	bool Draw();

private:
	// --- Draw
	void DrawMenuBar();
	void DrawToolbar();
	void DrawHierarchy();
	void DrawTile();
	void DrawMainPanel();

	// --- Events
	void OnImportTile(EventImportTile* e);

	//--- Utils
	String MaskToString(const BitArray& mask);

	bool TileButton(const char* name, bool selected, float width, float height);
	bool NeighbourCombo(const char* name, bool* selected, float texSize, unsigned int tex1, unsigned int tex2, unsigned int orientation);
	void HierarchyNode();


private:
	SceneTileManager* sceneTiles; // owned by MainScene (shared_ptr)

	// Tiles
	int currentTile = 0;
	int currentDir = 0;
	DynArray<TileData> tileData;

	// Flags
	bool isFilter = false;

	// Panel Data
	static const int menubarHeight = 19;
	static const int toolbarHeight = 46;
	static const int tileHeight = 136;
	static const int hierarchyWidth = 180;
};
