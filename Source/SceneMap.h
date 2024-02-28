#pragma once
#include "DynArray.h"

class MapGenerator;
class PathGenerator;
struct Tile;

class Camera;
class CameraController;
class UI_ButtonGroup;
class UI_Button;

class SceneMap
{
public:
	enum State {
		STOP = 0,
		PLAY,
		PAUSE,
		FINISHED
	};

public:
	SceneMap();
	~SceneMap();

	bool Init(const MapGenerator& map);
	bool Update(float dt);
	bool CleanUp();

	bool Draw(const MapGenerator& map);
	bool DrawUI(const MapGenerator& map, const PathGenerator& paths);

	// ---
	Camera* GetCamera() const { return camera; }
	State GetState() const { return state; }
	void SetState(State state) { this->state = state; }
	void UnselectAllCells();

	void AddTime(float time) { totalTime += time; }
	void SetStepTime(float time) { stepTime = time; }
	void SetPathsTime(float time) { pathsTime = time; }
	void SetTotalTime(float time) { totalTime = time; }

	void OnWindowResize(int width, int height);
	void OnZoom(float zoom);
	void OnMapResize(float width, float height);
	void OnPlay();
	void OnStep();
	void OnStop();

private:
	// ---
	void Shortcuts();
	void DrawSpaced();
	void PanelToggled(bool isOpen);
	void UpdateButtonGrid();
	void DrawMap(const MapGenerator& map);
	void DrawAreas(const MapGenerator& map, const PathGenerator& paths);

	// --- Draw
	void DrawPanel(const MapGenerator& map);
	void DrawSectionButtons();
	void DrawSectionOptions();
	void DrawSectionResize(int width, int height);
	void DrawSectionCellPresets(const MapGenerator& map);
	void DrawSectionCellInspector(const MapGenerator& map);
	void DrawCellInspector(const DynArray<Tile*>& tiles);

	// --- UI elements
	void DrawText(const char* label, const int maxSize);

private:
	// State
	State state = State::STOP;

	// Entities
	Camera* camera = nullptr;
	CameraController* controller = nullptr;
	UI_ButtonGroup* buttonGrid = nullptr;
	UI_Button* bgButton = nullptr;

	//  UI Variables
	static const int defaultSpacing = 2;
	float stepTime = 0;
	float pathsTime = 0;
	float totalTime = 0;
	float widthRatio = 1.0f;
	float heightRatio = 1.0f;
	int spacing = 0;
	bool isDrawSpaced = true;
	bool isDrawAreas = true;

	// Panel Data
	static const int menuBarHeight = 19;
	static const int widthOpen = 230;
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
};
