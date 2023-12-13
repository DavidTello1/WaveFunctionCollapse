#pragma once
#include "DynArray.h"
#include "BitArray.h"
#include "String.h"

struct Cell;
class SceneMapGenerator;

// --- Events
struct EventWindowResize;
struct EventMapResize;
struct EventPanelToggled;
// ---

class SceneMapGeneratorUI
{
public:
	SceneMapGeneratorUI(SceneMapGenerator* mapGenerator, const DynArray<unsigned int>& tileTextures);
	~SceneMapGeneratorUI();

	bool Init();
	bool Start();
	bool CleanUp();

	bool Draw(int width, int height, Cell* cell);

	// --- Getters
	bool IsDrawTextures() const { return isDrawTextures; }
	int GetPanelX() const { return panelX; }
	int GetPanelY() const { return panelY; }
	int GetPanelWidth() const { return panelWidth; }
	int GetPanelHeight() const { return panelHeight; }

	// --- Setters
	void SetTileTextures(const DynArray<unsigned int>& textures) { tileTextures = textures; }
	void AddTime(float time) { totalTime += time; }
	void SetStepTime(float time) { stepTime = time; }
	void SetTotalTime(float time) { totalTime = time; }

private:
	// --- Draw
	void DrawMenuBar();
	void DrawPanel(int width, int height, Cell* cell);
	void DrawSectionButtons();
	void DrawSectionOptions();
	void DrawSectionResize(int width, int height);
	void DrawSectionCellPresets();
	void DrawSectionCellInspector(int width, Cell* cell);
	void DrawCellInspector(const DynArray<unsigned int>& tileIDs);

	// --- Events
	void OnWindowResize(EventWindowResize* e);
	void OnMapResize(EventMapResize* e);
	void OnPanelToggled(EventPanelToggled* e);

	//--- Utils
	String MaskToString(const BitArray& mask);

private:
	SceneMapGenerator* sceneMap; // owned by MainScene (shared_ptr)

	// Timer
	float stepTime = 0;
	float totalTime = 0;

	// Size
	float widthRatio = 1.0f;
	float heightRatio = 1.0f;

	// Flags
	bool isDrawTextures = true;
	bool isDrawSpaced = true;

	// Panel Data
	static const int menuBarHeight = 19;
	static const int widthOpen = 220;
	static const int widthClosed = 20;
	bool isPanelOpen = true;
	int panelX = 0;
	int panelY = 0;
	int panelWidth = 0;
	int panelHeight = 0;

	// Icons
	unsigned int stepIcon = 0;
	unsigned int playIcon = 0;
	unsigned int stopIcon = 0;
	unsigned int restartIcon = 0;

	// Tile Textures
	DynArray<unsigned int> tileTextures;
};
