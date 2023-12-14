#pragma once
#include "Scene.h"

#include "PerfTimer.h"

class MapGenerator;
class TileManager;

class SceneMapGenerator;
class SceneMapGeneratorUI;
class SceneTileManagerUI;

// --- Events
struct EventPlay;
struct EventStep;
struct EventStop;
struct EventSetCell;
struct EventPresetCells;
struct EventResetCells;
struct EventResetAllCells;
struct EventMapResize;
struct EventDrawSpaced;
struct EventChangeScene;
struct EventSaveTileset;
// ---

class MainScene : public Scene
{
public:
	MainScene();
	virtual ~MainScene();

	bool Init() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	bool Draw() override;
	bool DrawUI() override;

private:
	void DrawMap();

	// --- EVENTS ---
	void OnPlay(EventPlay* e);
	void OnStep(EventStep* e);
	void OnStop(EventStop* e);

	void OnSetCell(EventSetCell* e);
	void OnPresetCells(EventPresetCells* e);
	void OnResetCells(EventResetCells* e);
	void OnResetAllCells(EventResetAllCells* e);

	void OnMapResize(EventMapResize* e);
	void OnDrawSpaced(EventDrawSpaced* e);
	void OnChangeScene(EventChangeScene* e);

	void OnSaveTileset(EventSaveTileset* e);

public:
	bool isMapVisible = true;

	// --- MAP GENERATOR ---
	MapGenerator* mapGenerator = nullptr;
	int width = 25;
	int height = 25;

	// --- Map Drawing
	int cellSize = 24;
	int spacing = 2;

	// --- Timer
	PerfTimer timer;

	// --- TILE MANAGER ---
	TileManager* tileManager = nullptr;

	// --- SCENES ---
	SceneMapGenerator* sceneMapGenerator = nullptr;
	SceneMapGeneratorUI* sceneMapGeneratorUI = nullptr;
	SceneTileManagerUI* sceneTileManagerUI = nullptr;
};
