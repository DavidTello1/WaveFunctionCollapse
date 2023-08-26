#pragma once
#include "Scene.h"

#include "List.h"

class MapGenerator;
class ButtonGrid;

class SceneMapGenerator : public Scene
{
private:
	enum State {
		STOP = 0,
		PLAY,
		PAUSE,
		FINISHED
	};

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
	// --- Debug Utils
	void PresetCells(const List<unsigned int>& cells, const unsigned int tileID);
	void ClearCells(const List<unsigned int>& cells);
	void UpdateButtonsPosition();

	// --- State Management
	void Play();
	void Step();
	void Stop();
	void Restart();

	// --- Debug Draw
	void DrawPanel();
	void DrawSectionButtons();
	void DrawSectionOptions();
	void DrawSectionResize();
	void DrawSectionCellPresets();
	void DrawSectionCellInspector();
	void DrawCellInspector(const List<unsigned int>& list);

private:
	MapGenerator* map = nullptr;

	// --- Map Data
	static const int numTiles = 7;
	static const int cellSize = 24;
	static const int spacing = 2;
	int width = 25;
	int height = 25;

	// --- Debug
	State state = State::STOP;
	ButtonGrid* buttonGrid = nullptr;

	bool isMapPreset = false;
	bool isDrawTextures = true;
	bool isDrawSpaced = true;
	
	unsigned int stepIcon = 0;
	unsigned int playIcon = 0;
	unsigned int stopIcon = 0;
	unsigned int restartIcon = 0;
};

