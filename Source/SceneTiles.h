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

class SceneTiles
{
public:
	SceneTiles();
	~SceneTiles();

	bool Init();
	bool Update(float dt);
	bool CleanUp();

	bool DrawUI(const Tileset* tileset);

	// ---
	const TileData& GetTileData(int index) const { return tileData[index]; }
	void ImportTile(unsigned int tileID, const char* name, const char* texturePath);
	void ClearTileData();
	bool HasChanges() const { return isChanges; }
	void SetChanges(bool value) { isChanges = value; }

private:
	// --- Draw
	void DrawToolbar();
	void DrawHierarchy();
	void DrawMainPanel(const Tileset* tileset);
	void DrawTileData(const Tileset* tilese, const float panelWidth);
	void DrawMask(const char* name, bool* selected, const int dir, const Tileset* tileset, const int size, const float panelWidth);
	void DrawCombinations(const Tileset* tileset, const float panelWidth);

	//--- Utils
	bool ToolbarIcon(unsigned int icon, const int buttonSize, const char* tooltip, bool selected = false);
	bool NeighbourCombo(const char* name, bool selected, float texSize, unsigned int tex1, unsigned int tex2, unsigned int orientation, bool rclick_hover = false);
	bool HierarchyNode(const char* name, bool selected, bool rename, bool changes);
	String CreateName();
	bool ExistsName(const char* name);
	void Shortcuts();

private:
	// Tiles Data
	DynArray<TileData> tileData;
	int currentTile = 0;
	int renameNode = -1;

	// Flags
	bool isFilter = false;
	bool isChanges = false;
	bool isTileData = true;
	bool isRenameFocus = false;

	// Panel Data
	static const int menubarHeight = 19;
	static const int toolbarHeight = 46;
	static const int hierarchyWidth = 180;

	// Icons
	unsigned int saveIcon = 0;
	unsigned int saveAllIcon = 0;
	unsigned int importIcon = 0;
	unsigned int filterIcon = 0;
	unsigned int comboIcon = 0;

	// --- (used for right click options in DrawTileData)
	struct ComboData {
		int dir = -1;
		int firstIndex = -1;
		int secondIndex = -1;
	} comboData;
};
