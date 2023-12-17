#pragma once
#include "DynArray.h"
#include "String.h"

class Tileset;

struct TileData {
	String name;
	String texturePath;
	int tileID;

	bool isChanged;
};

class SceneTileManagerUI
{
public:
	SceneTileManagerUI();
	~SceneTileManagerUI();

	bool Init();
	bool CleanUp();

	bool DrawUI(const Tileset* tileset);

	// ---
	void ImportTile(unsigned int tileID, const char* name, const char* texturePath);

private:
	// --- Draw
	void DrawMenuBar();
	void DrawToolbar();
	void DrawHierarchy();
	void DrawTile(unsigned int texture);
	void DrawMainPanel(const Tileset* tileset);

	//--- Utils
	bool TileButton(const char* name, bool selected, float width, float height);
	bool NeighbourCombo(const char* name, bool selected, float texSize, unsigned int tex1, unsigned int tex2, unsigned int orientation);
	bool HierarchyNode(const char* name, bool selected, bool rename, bool changes);

private:
	// Tiles Data
	DynArray<TileData> tileData;
	int currentTile = 0;
	int currentDir = 0;
	int renameNode = -1;

	// Flags
	bool isFilter = false;
	bool isChanges = false;
	bool isRenameFocus = false;

	// Panel Data
	static const int menubarHeight = 19;
	static const int toolbarHeight = 46;
	static const int tileHeight = 136;
	static const int hierarchyWidth = 180;

	// Icons
	unsigned int saveIcon = 0;
	unsigned int saveAllIcon = 0;
	unsigned int importIcon = 0;
	unsigned int filterIcon = 0;
};
