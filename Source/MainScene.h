#pragma once
#include "Scene.h"

#include "json/json.hpp"
#include "PerfTimer.h"

typedef nlohmann::json json;

class MapGenerator;
class Tileset;

class SceneMap;
class SceneTiles;

// --- Events
struct EventWindowResize;
struct EventCameraZoom;

struct EventImportAny;
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
struct EventRemoveTile;
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
	void ImportTile(const char* texturePath);
	void ImportTileset(json& file);
	void ExportTileset(json& file);
	void ImportMap(json& file);
	void ExportMap(json& file);

	// --- EVENTS ---
	void OnWindowResize(EventWindowResize* e);
	void OnZoom(EventCameraZoom* e);

	void OnImportAny(EventImportAny* e);
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
	void OnRemoveTile(EventRemoveTile* e);
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
	SceneMap* sceneMap = nullptr;
	SceneTiles* sceneTiles = nullptr;
};
