#pragma once
#include "Scene.h"

#include "PerfTimer.h"

class MapGenerator;
class Tileset;

class SceneMapGeneratorUI;
class SceneTileManagerUI;

// --- Events
struct EventWindowResize;
struct EventCameraZoom;

struct EventImportTile;
struct EventImportTileset;
struct EventImportMap;
struct EventExportTileset;
struct EventExportMap;

struct EventPlay;
struct EventStep;
struct EventStop;
struct EventSetCell;
struct EventPresetCells;
struct EventResetCells;
struct EventResetAllCells;
struct EventMapResize;
struct EventChangeScene;

struct EventSaveTileset;
struct EventUpdateMask;
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
	// --- EVENTS ---
	void OnWindowResize(EventWindowResize* e);
	void OnZoom(EventCameraZoom* e);

	void OnImportTile(EventImportTile* e);
	void OnImportTileset(EventImportTileset* e);
	void OnImportMap(EventImportMap* e);
	void OnExportTileset(EventExportTileset* e);
	void OnExportMap(EventExportMap* e);

	void OnPlay(EventPlay* e);
	void OnStep(EventStep* e);
	void OnStop(EventStop* e);

	void OnSetCell(EventSetCell* e);
	void OnPresetCells(EventPresetCells* e);
	void OnResetCells(EventResetCells* e);
	void OnResetAllCells(EventResetAllCells* e);

	void OnMapResize(EventMapResize* e);
	void OnChangeScene(EventChangeScene* e);

	void OnSaveTileset(EventSaveTileset* e);
	void OnUpdateMask(EventUpdateMask* e);

public:
	bool isSceneMap = true;

	// --- Map Generator
	MapGenerator* mapGenerator = nullptr;
	int width = 25;
	int height = 25;
	int cellSize = 24;
	PerfTimer timer;

	// --- Tile Manager
	Tileset* tileset = nullptr;

	// --- Scenes
	SceneMapGeneratorUI* sceneMapGeneratorUI = nullptr;
	SceneTileManagerUI* sceneTileManagerUI = nullptr;
};
