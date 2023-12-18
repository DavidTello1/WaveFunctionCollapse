#include "MainScene.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"
#include "ModuleResources.h" //***

#include "MapGenerator.h"
#include "Tileset.h"
#include "Tile.h"
#include "Cell.h"

#include "SceneMap.h"
#include "SceneTiles.h"

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

    App->event->Subscribe(this, &MainScene::OnImportTile);
    App->event->Subscribe(this, &MainScene::OnImportTileset);
    App->event->Subscribe(this, &MainScene::OnImportMap);
    App->event->Subscribe(this, &MainScene::OnExportTileset);
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
    Tile* empty = new Tile(0, App->resources->LoadTexture("Assets/Textures/empty.png")->index, "1001111", "1010111", "1101011", "1110011");
    Tile* topLeft = new Tile(1, App->resources->LoadTexture("Assets/Textures/topLeft.png")->index, "1000000", "1000000", "0010110", "0001101");
    Tile* topRight = new Tile(2, App->resources->LoadTexture("Assets/Textures/topRight.png")->index, "1000000", "0101010", "1000000", "0001101");
    Tile* bottomLeft = new Tile(3, App->resources->LoadTexture("Assets/Textures/bottomLeft.png")->index, "0110001", "1000000", "0010110", "1000000");
    Tile* bottomRight = new Tile(4, App->resources->LoadTexture("Assets/Textures/bottomRight.png")->index, "0110001", "0101010", "1000000", "1000000");
    Tile* horizontal = new Tile(5, App->resources->LoadTexture("Assets/Textures/horizontal.png")->index, "1000000", "1101010", "1010110", "1000000");
    Tile* vertical = new Tile(6, App->resources->LoadTexture("Assets/Textures/vertical.png")->index, "1110001", "1000000", "1000000", "1001101");

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
// --- EVENTS ---
void MainScene::OnWindowResize(EventWindowResize* e)
{
    sceneMap->OnWindowResize(e->width, e->height);
}

void MainScene::OnZoom(EventCameraZoom* e)
{
    sceneMap->OnZoom(e->zoom);
}

void MainScene::OnImportTile(EventImportTile* e)
{
    //***
}

void MainScene::OnImportTileset(EventImportTileset* e)
{
    //json file = App->resources->LoadJson(e->filepath);

    //if (file.find("Tileset") == file.end())
    //{
    //    LOG("Error importing tileset, not found in json file");
    //}

    //json tileset = file["Tileset"];
    //for (json::iterator it = tileset.begin(); it != tileset.end(); ++it)
    //{
    //    json name = tileset[it.key()]["name"];
    //    json id = tileset[it.key()]["ID"];
    //    json texturePath = tileset[it.key()]["texturePath"];
    //    json topMask = tileset[it.key()]["topMask"];
    //    json leftMask = tileset[it.key()]["leftMask"];
    //    json rightMask = tileset[it.key()]["rightMask"];
    //    json bottomMask = tileset[it.key()]["bottomMask"];

    //    unsigned int texture = App->resources->LoadTexture(texturePath)->index;
    //    Tile* tile = new Tile(id, texture, topMask, leftMask, rightMask, bottomMask);
    //    this->tileset->AddTile(tile);
    //    OnImportTile(&EventImportTile(name, texturePath, id));
    //}
}

void MainScene::OnImportMap(EventImportMap* e)
{
    //// --- Import Tileset
    //App->event->Publish(new EventImportTileset(e->filepath));
    //const DynArray<Tile*> tileset = sceneTileManagerUI->GetTileset();
    //mapGenerator->SetTileset(tileset);

    //// --- Import Map
    //json file = App->resources->LoadJson(e->filepath);

    //if (file.find("Map") == file.end())
    //{
    //    LOG("Error importing map, not found in json file");
    //    return;
    //}
    //json map = file["Map"];

    //// Map Data
    //width    = map["data"]["width"];
    //height   = map["data"]["height"];
    //cellSize = map["data"]["cellSize"];
    //mapGenerator->SetSize(width, height);
    //mapGenerator->SetCellSize(cellSize); //***

    //// Preset Cells
    //json cells = map["cells"];
    //for (json::iterator it = cells.begin(); it != cells.end(); ++it)
    //{
    //    json tile   = cells[it.key()]["tileID"];
    //    json index  = cells[it.key()]["index"];

    //    mapGenerator->PresetCell(index, tile);
    //}
}

void MainScene::OnExportTileset(EventExportTileset* e)
{
    //json file;

    //for (int i = 0; i < tileData.size(); ++i)
    //{
    //    Tile* tile = tileset->GetTile(i);

    //    file["Tileset"][tileData[i].name]["name"] = tileData[i].name;
    //    file["Tileset"][tileData[i].name]["ID"] = tileData[i].tileID;
    //    file["Tileset"][tileData[i].name]["texturePath"] = tileData[i].texturePath;
    //    file["Tileset"][tileData[i].name]["topMask"] = tile->GetMasks()[0];
    //    file["Tileset"][tileData[i].name]["leftMask"] = tile->GetMasks()[1];
    //    file["Tileset"][tileData[i].name]["rightMask"] = tile->GetMasks()[2];
    //    file["Tileset"][tileData[i].name]["bottomMask"] = tile->GetMasks()[3];
    //}

    //// --- Serialize JSON to string ---
    //App->resources->SaveJson(e->filepath, file);
}

void MainScene::OnExportMap(EventExportMap* e)
{
    ////json node;

    ////Tileset* tileset = sceneTileManagerUI->GetTileset();

    ////for (int i = 0; i < animator->animations.size(); ++i)
    ////{
    ////    node[animator->animations[i]->name]["name"] = animator->animations[i]->name;
    ////    node[animator->animations[i]->name]["start_frame"] = std::to_string(animator->animations[i]->start);
    ////    node[animator->animations[i]->name]["end_frame"] = std::to_string(animator->animations[i]->end);
    ////    node[animator->animations[i]->name]["loop"] = animator->animations[i]->loop;
    ////    node[animator->animations[i]->name]["default"] = animator->animations[i]->Default;
    ////    //node[animator->animations[i]->name]["speed"] = animator->animations[i]->speed;
    ////}

    ////// --- Serialize JSON to string ---
    ////std::string data;
    ////App->filesystem->Serialize(node, data);

    ////// --- Finally Save to file ---
    ////char* buffer = (char*)data.data();
    ////unsigned int size = data.length();

    ////String path = "";
    ////App->filesystem->Save(path, buffer, size);
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
    for (unsigned int i = 0; i < numCells; ++i)
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

void MainScene::OnUpdateMask(EventUpdateMask* e)
{
    tileset->UpdateMask(e->index, e->dir, e->bit, e->value);
}
