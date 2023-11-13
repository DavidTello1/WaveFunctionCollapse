#pragma once
#include "Scene.h"

#include "List.h"

class MapGenerator;
class CameraController;

class ButtonGrid;
class UI_Button;

// --- Events
struct EventWindowResize;
struct EventCameraZoom;
// ---

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
	void UpdateButtonGrid();
	void MapResized(const int mapWidth, const int mapHeight);
	void PanelToggled(bool isOpen);

	// --- State Management
	void Play();
	void Step();
	void Stop();
	void Restart();

	// --- Debug Draw
	void DrawMenuBar();
	void DrawPanel();
	void DrawSectionButtons();
	void DrawSectionOptions();
	void DrawSectionResize();
	void DrawSectionCellPresets();
	void DrawSectionCellInspector();
	void DrawCellInspector(const List<unsigned int>& list);

	// --- EVENTS ---
	void OnResize(EventWindowResize* e);
	void OnZoom(EventCameraZoom* e);

private:
	// --- Scene Data
	MapGenerator* map = nullptr;
	CameraController* controller = nullptr;

	// --- Map Data
	static const int numTiles = 7;
	static const int cellSize = 24;
	static const int spacing = 2;
	static const int menuBarHeight = 19;
	int width = 25;
	int height = 25;

	// --- Debug
	State state = State::STOP;
	ButtonGrid* buttonGrid = nullptr;
	UI_Button* bgButton = nullptr;

	float widthRatio = 1.0f;
	float heightRatio = 1.0f;

	bool isMapPreset = false;
	bool isDrawTextures = true;
	bool isDrawSpaced = true;

	bool isPanelOpen = true;
	int panelX = 0;
	int panelWidth = 0;
	int panelHeight = 0;
	
	unsigned int stepIcon = 0;
	unsigned int playIcon = 0;
	unsigned int stopIcon = 0;
	unsigned int restartIcon = 0;
};
