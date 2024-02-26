#pragma once
#include "Scene.h"

#include "json/json.hpp"
#include "PerfTimer.h"

typedef nlohmann::json json;

class MapGenerator;
class PathGenerator;
class Tileset;

class SceneMap;
class SceneTiles;

// --- Events
struct EventWindowResize;
struct EventCameraZoom;

struct EventPlay;
struct EventStep;
struct EventStop;
struct EventSetCell;
struct EventPresetCells;
struct EventResetCells;
struct EventResetAllCells;
struct EventMapResize;

struct EventImport;
struct EventSaveTileset;
struct EventRemoveTile;
struct EventUpdateMask;
struct EventSetTileWalkable;
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
	void DrawMenuBar();
	void Shortcuts();

	void OpenFileDialog(const char* title, const char* defaultFolder, const int numFilters, const char** filters, const char* filterDesc, bool isMultiSelect);
	void ImportFile(const char* filepath);
	void OnImportTile();
	void OnImportTileset();
	void OnImportMap();
	void OnExportTileset();
	void OnExportMap();

	void ImportTile(const char* texturePath);
	void ImportTileset(json& file);
	void ExportTileset(json& file);
	void ImportMap(json& file);
	void ExportMap(json& file);

	// --- EVENTS ---
	void OnWindowResize(EventWindowResize* e);
	void OnZoom(EventCameraZoom* e);

	void OnImportAny(EventImport* e);

	void OnPlay(EventPlay* e);
	void OnStep(EventStep* e);
	void OnStop(EventStop* e);

	void OnSetCell(EventSetCell* e);
	void OnPresetCells(EventPresetCells* e);
	void OnResetCells(EventResetCells* e);
	void OnResetAllCells(EventResetAllCells* e);

	void OnMapResize(EventMapResize* e);

	void OnSaveTileset(EventSaveTileset* e);
	void OnRemoveTile(EventRemoveTile* e);
	void OnUpdateMask(EventUpdateMask* e);
	void OnSetTileWalkable(EventSetTileWalkable* e);

public:
	bool isSceneMap = true;

	// --- Map Generator
	MapGenerator* mapGenerator = nullptr;
	PathGenerator* pathGenerator = nullptr;
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
