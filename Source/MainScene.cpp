#include "MainScene.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "MapGenerator.h"
#include "Tileset.h"
#include "Tile.h"
#include "Cell.h"

#include "SceneMap.h"
#include "SceneTiles.h"

#include "FileDialog.h"
#include "Utils.h"
#include "Color.h"

#include "mmgr/mmgr.h"

MainScene::MainScene() : Scene("MainScene")
{
}

MainScene::~MainScene()
{
}

bool MainScene::Init()
{
    isSceneMap = false;

    // --- Init Map Data
    width = 25;
    height = 25;
    cellSize = 24;

    // --- Create Tileset
    tileset = new Tileset();

    // --- Create Map Generator
    mapGenerator = new MapGenerator(width, height, cellSize);

    // --- Create Scenes
    sceneMap = new SceneMap();
    sceneTiles = new SceneTiles();

    // --- Events
    App->event->Subscribe(this, &MainScene::OnWindowResize);
    App->event->Subscribe(this, &MainScene::OnZoom);

    App->event->Subscribe(this, &MainScene::OnImportAny);
    App->event->Subscribe(this, &MainScene::OnImportTile);
    App->event->Subscribe(this, &MainScene::OnImportTileset);
    App->event->Subscribe(this, &MainScene::OnExportTileset);
    App->event->Subscribe(this, &MainScene::OnImportMap);
    App->event->Subscribe(this, &MainScene::OnExportMap);

    App->event->Subscribe(this, &MainScene::OnPlay);
    App->event->Subscribe(this, &MainScene::OnStep);
    App->event->Subscribe(this, &MainScene::OnStop);

    App->event->Subscribe(this, &MainScene::OnSetCell);
    App->event->Subscribe(this, &MainScene::OnPresetCells);
    App->event->Subscribe(this, &MainScene::OnResetCells);
    App->event->Subscribe(this, &MainScene::OnResetAllCells);

    App->event->Subscribe(this, &MainScene::OnMapResize);
    App->event->Subscribe(this, &MainScene::OnChangeScene);

    App->event->Subscribe(this, &MainScene::OnSaveTileset);
    App->event->Subscribe(this, &MainScene::OnRemoveTile);
    App->event->Subscribe(this, &MainScene::OnUpdateMask);

    return true;
}

bool MainScene::Start()
{
    // --- Init Scenes
    sceneMap->Init(mapGenerator);
    sceneTiles->Init();

    // Set Scene Camera (created in sceneMap->Init())
    this->camera = sceneMap->GetCamera();

    //***
    // -----------------------------------------
    Tile empty       = Tile(0, App->resources->LoadTexture("Assets/Textures/empty.png")->index,       "1001111", "1010111", "1101011", "1110011");
    Tile topLeft     = Tile(1, App->resources->LoadTexture("Assets/Textures/topLeft.png")->index,     "1000000", "1000000", "0010110", "0001101");
    Tile topRight    = Tile(2, App->resources->LoadTexture("Assets/Textures/topRight.png")->index,    "1000000", "0101010", "1000000", "0001101");
    Tile bottomLeft  = Tile(3, App->resources->LoadTexture("Assets/Textures/bottomLeft.png")->index,  "0110001", "1000000", "0010110", "1000000");
    Tile bottomRight = Tile(4, App->resources->LoadTexture("Assets/Textures/bottomRight.png")->index, "0110001", "0101010", "1000000", "1000000");
    Tile horizontal  = Tile(5, App->resources->LoadTexture("Assets/Textures/horizontal.png")->index,  "1000000", "1101010", "1010110", "1000000");
    Tile vertical    = Tile(6, App->resources->LoadTexture("Assets/Textures/vertical.png")->index,    "1110001", "1000000", "1000000", "1001101");

    tileset->AddTile(empty);
    tileset->AddTile(topLeft);
    tileset->AddTile(topRight);
    tileset->AddTile(bottomLeft);
    tileset->AddTile(bottomRight);
    tileset->AddTile(horizontal);
    tileset->AddTile(vertical);

    sceneTiles->ImportTile(0, "Empty", "Assets/Textures/empty.png");
    sceneTiles->ImportTile(1, "TopLeft", "Assets/Textures/topLeft.png");
    sceneTiles->ImportTile(2, "TopRight", "Assets/Textures/topRight.png");
    sceneTiles->ImportTile(3, "BottomLeft", "Assets/Textures/bottomLeft.png");
    sceneTiles->ImportTile(4, "BottomRight", "Assets/Textures/bottomRight.png");
    sceneTiles->ImportTile(5, "Horizontal", "Assets/Textures/horizontal.png");
    sceneTiles->ImportTile(6, "Vertical", "Assets/Textures/vertical.png");
    //----------------------------------------

    return true;
}

bool MainScene::Update(float dt)
{
    if (isSceneMap)
    {
        if (mapGenerator->IsFinished())
            sceneMap->SetState(SceneMap::State::FINISHED);

        // Update Scenes
        sceneMap->Update(dt);
    }

    return true;
}

bool MainScene::CleanUp()
{
    sceneMap->CleanUp();
    sceneTiles->CleanUp();

    delete sceneMap;
    delete sceneTiles;

    delete tileset;
    delete mapGenerator;

    return true;
}

bool MainScene::Draw()
{
    if (isSceneMap)
        sceneMap->Draw(mapGenerator);

    return true;
}

bool MainScene::DrawUI()
{
    if (isSceneMap)
        sceneMap->DrawUI(mapGenerator);
    else
        sceneTiles->DrawUI(tileset);

    return true;
}

// -------------------------------
void MainScene::ImportTile(const char* path)
{
    int tileID = App->GenerateID();
    unsigned int texture = App->resources->LoadTexture(path)->index;
    std::string name = App->filesystem->GetFileName(path, false);

    tileset->AddTile(tileID, texture);
    sceneTiles->ImportTile(tileID, name.c_str(), path);
}

void MainScene::ImportTileset(json& file)
{
    if (file.find("tileset") == file.end())
    {
        LOG("Error importing tileset, not found in json file");
        return;
    }

    for (json::iterator it = file["tileset"].begin(); it != file["tileset"].end(); ++it)
    {
        // Read Tile
        std::string name        = file["tileset"][it.key()]["name"];
        int id                  = file["tileset"][it.key()]["ID"];
        std::string texturePath = file["tileset"][it.key()]["texturePath"];
        std::string topMask     = file["tileset"][it.key()]["topMask"];
        std::string leftMask    = file["tileset"][it.key()]["leftMask"];
        std::string rightMask   = file["tileset"][it.key()]["rightMask"];
        std::string bottomMask  = file["tileset"][it.key()]["bottomMask"];

        // Create Tile
        unsigned int texture = App->resources->LoadTexture(texturePath.c_str())->index;
        Tile tile = Tile(id, texture, topMask.c_str(), leftMask.c_str(), rightMask.c_str(), bottomMask.c_str());

        tileset->AddTile(tile);
        sceneTiles->ImportTile(id, name.c_str(), texturePath.c_str());
    }
}

void MainScene::ExportTileset(json& file)
{
    for (int i = 0; i < tileset->GetSize(); ++i)
    {
        const Tile* tile = tileset->GetTile(i);
        const TileData data = sceneTiles->GetTileData(i);

        file["tileset"][data.name.c_str()]["name"]        = data.name.c_str();
        file["tileset"][data.name.c_str()]["ID"]          = tile->GetID();
        file["tileset"][data.name.c_str()]["texturePath"] = data.texturePath.c_str();
        file["tileset"][data.name.c_str()]["topMask"]     = Utils::MaskToString(tile->GetMasks()[0]).c_str();
        file["tileset"][data.name.c_str()]["leftMask"]    = Utils::MaskToString(tile->GetMasks()[1]).c_str();
        file["tileset"][data.name.c_str()]["rightMask"]   = Utils::MaskToString(tile->GetMasks()[2]).c_str();
        file["tileset"][data.name.c_str()]["bottomMask"]  = Utils::MaskToString(tile->GetMasks()[3]).c_str();
    }
}

void MainScene::ImportMap(json& file)
{
    // --- Import Tileset
    ImportTileset(file);

    // --- Import Map
    if (file.find("map") == file.end())
    {
        LOG("Error importing map, not found in json file");
        return;
    }

    // Map Data
    width = file["map"]["data"]["width"];
    height = file["map"]["data"]["height"];
    cellSize = file["map"]["data"]["cellSize"];

    mapGenerator->SetSize(width, height);
    mapGenerator->SetCellSize(cellSize);

    // Preset Cells
    for (json::iterator it = file["cells"].begin(); it != file["cells"].end(); ++it)
    {
        json tile = file["map"]["cells"][it.key()]["tileID"];
        json index = file["map"]["cells"][it.key()]["index"];

        mapGenerator->PresetCell(index, tile);
    }
}

void MainScene::ExportMap(json& file)
{
    // --- Export Tileset
    ExportTileset(file);

    // --- Export Map
    // Map Data
    file["map"]["data"]["width"] = width;
    file["map"]["data"]["height"] = height;
    file["map"]["data"]["cellSize"] = cellSize;

    // Preset Cells
    int numCells = width * height;
    for (int i = 0; i < numCells; ++i)
    {
        Cell* cell = mapGenerator->GetCell(i);
        if (!cell->isPreset)
            continue;

        file["cells"][i]["tileID"] = cell->tileID;
        file["cells"][i]["index"] = cell->index;
    }
}

// -------------------------------
// --- EVENTS ---
void MainScene::OnWindowResize(EventWindowResize* e)
{
    sceneMap->OnWindowResize(e->width, e->height);
}

void MainScene::OnZoom(EventCameraZoom* e)
{
    sceneMap->OnZoom(e->zoom);
}

void MainScene::OnImportAny(EventImportAny* e)
{
    wchar_t  filepath[MAX_PATH];
    filepath[0] = 0;
    COMDLG_FILTERSPEC filters[2] = {
        {L"Files", L"*.json;*.jpg;*.png;*.bmp;*.tif"},
        {L"All Files", L"*.*"}
    };

    std::string path = FileDialog::OpenFileDialog(filepath, 2, filters);
    if (path == "")
        return;
    App->filesystem->NormalizePath(path);

    std::string extension = App->filesystem->GetExtension(path.c_str());
    if (extension == "json")
    {
        json file = App->resources->LoadJson(path.c_str());
        ImportMap(file);
    }
    else if (extension == "jpg" || extension == "png" || extension == "bmp" || extension == "tif")
    {
        ImportTile(path.c_str());
    }
}

void MainScene::OnImportTile(EventImportTile* e)
{
    wchar_t  filepath[MAX_PATH];
    filepath[0] = 0;
    COMDLG_FILTERSPEC filters[2] = {
        {L"Image Files", L"*.jpg;*.png;*.bmp;*.tif"},
        {L"All Files", L"*.*"}
    };

    std::string path = FileDialog::OpenFileDialog(filepath, 2, filters);
    if (path == "")
        return;
    App->filesystem->NormalizePath(path);

    ImportTile(path.c_str());
}

void MainScene::OnImportTileset(EventImportTileset* e)
{
    wchar_t  filepath[MAX_PATH];
    filepath[0] = 0;
    COMDLG_FILTERSPEC filters[2] = {
        {L"Json Files", L"*.json"},
        {L"All Files", L"*.*"}
    };

    std::string path = FileDialog::OpenFileDialog(filepath, 2, filters);
    if (path == "")
        return;
    App->filesystem->NormalizePath(path);

    // ---
    json file = App->resources->LoadJson(path.c_str());
    ImportTileset(file);
}

void MainScene::OnImportMap(EventImportMap* e)
{
    wchar_t  filepath[MAX_PATH];
    filepath[0] = 0;
    COMDLG_FILTERSPEC filters[2] = {
        {L"Json Files", L"*.json"},
        {L"All Files",L"*.*"}
    };

    std::string path = FileDialog::OpenFileDialog(filepath, 2, filters);
    if (path == "")
        return;
    App->filesystem->NormalizePath(path);

    // ---
    json file = App->resources->LoadJson(path.c_str());
    ImportMap(file);
}

void MainScene::OnExportTileset(EventExportTileset* e)
{
    ////FileDialog::SaveFileDialog();
    //String path = "";

    //json file;
    //ExportTileset(file);

    //App->resources->SaveJson(path.c_str(), file);
}

void MainScene::OnExportMap(EventExportMap* e)
{
    ////FileDialog::SaveFileDialog();
    //String path = "";

    //json file;
    //ExportMap(file);

    //App->resources->SaveJson(path.c_str(), file);
}

void MainScene::OnPlay(EventPlay* e)
{
    timer.Start();

    mapGenerator->GenerateMap();

    sceneMap->AddTime(timer.ReadMs());

    sceneMap->OnPlay();
}

void MainScene::OnStep(EventStep* e)
{
    timer.Start();

    mapGenerator->Step();

    float time = timer.ReadMs();
    sceneMap->SetStepTime(time);
    sceneMap->AddTime(time);

    sceneMap->OnStep();
}

void MainScene::OnStop(EventStop* e)
{
    mapGenerator->ResetMap();

    sceneMap->SetStepTime(0.0f);
    sceneMap->SetTotalTime(0.0f);

    sceneMap->OnStop();
}

void MainScene::OnSetCell(EventSetCell* e)
{
    mapGenerator->SetCell(e->cell, e->tileID);

    sceneMap->UnselectAllCells();
}

void MainScene::OnPresetCells(EventPresetCells* e)
{
    for (unsigned int i = 0; i < e->cells.size(); ++i)
    {
        int index = e->cells.at(i);
        mapGenerator->PresetCell(index, e->tileID);
    }

    sceneMap->UnselectAllCells();
}

void MainScene::OnResetCells(EventResetCells* e)
{
    for (unsigned int i = 0; i < e->cells.size(); ++i)
    {
        int index = e->cells.at(i);
        mapGenerator->ResetCell(index);
    }

    sceneMap->UnselectAllCells();
}

void MainScene::OnResetAllCells(EventResetAllCells* e)
{
    mapGenerator->ClearPresetCells();

    sceneMap->UnselectAllCells();
}

void MainScene::OnMapResize(EventMapResize* e)
{
    width = e->width;
    height = e->height;

    mapGenerator->SetSize(e->width, e->height);

    sceneMap->OnMapResize(e->width, e->height);
}

void MainScene::OnChangeScene(EventChangeScene* e)
{
    if (e->scene == "MapGenerator")
    {
        isSceneMap = true;
    }
    else if (e->scene == "TileManager")
    {
        isSceneMap = false;
    }
}

void MainScene::OnSaveTileset(EventSaveTileset* e) //***
{
    sceneMap->SetState(SceneMap::State::STOP);

    // Save the preset cells
    int numCells = width * height;
    DynArray<Cell> cells = DynArray<Cell>(numCells);
    for (int i = 0; i < numCells; ++i)
    {
        Cell* cell = mapGenerator->GetCell(i);
        if (cell->isPreset)
            cells.push_back(*cell);
    }

    // Update Tileset
    mapGenerator->SetTileset(tileset->GetAllTiles());

    // Apply preset Cells
    for (unsigned int i = 0; i < cells.size(); ++i)
    {
        int id = cells[i].tileID;
        if (tileset->IsValid(id))
            mapGenerator->PresetCell(cells[i].index, id);
    }
}

void MainScene::OnRemoveTile(EventRemoveTile* e)
{
    tileset->RemoveTile(e->index);
}

void MainScene::OnUpdateMask(EventUpdateMask* e)
{
    tileset->UpdateMask(e->index, e->dir, e->bit, e->value);
}
