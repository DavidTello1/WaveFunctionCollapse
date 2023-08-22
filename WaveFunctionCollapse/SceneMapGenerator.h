#pragma once
#include "Scene.h"

#include "List.h"

class MapGenerator;
class ButtonGrid;

class SceneMapGenerator : public Scene
{
public:
	SceneMapGenerator();
	virtual ~SceneMapGenerator();

	bool Init() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	bool Draw() override;
	bool DrawUI() override;

private:
	void PresetCells(List<unsigned int> cells, unsigned int tileID);

	void DrawPanel();
	void DrawCellInspector();

	void UpdateButtonsPosition();

private:
	MapGenerator* map = nullptr;

	// --- Map Data
	static const int numTiles = 7;
	static const int cellSize = 24;
	static const int spacing = 2;
	int width = 25;
	int height = 25;

	// --- Debug
	ButtonGrid* buttonGrid = nullptr;

	bool isPlay = false; //*** mask
	bool isDrawTextures = true; //***
	bool isDrawSpaced = true; //***
	
	unsigned int stepIcon = 0;
	unsigned int playIcon = 0;
	unsigned int restartIcon = 0;
};

