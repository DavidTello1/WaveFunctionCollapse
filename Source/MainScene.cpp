#include "MainScene.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"

#include "MapGenerator.h"
#include "Tile.h"
#include "Cell.h"

//#include "TileManager.h"

#include "SceneMapGenerator.h"
#include "SceneMapGeneratorUI.h"
#include "SceneTileManagerUI.h"

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
    isMapVisible = false;

    // --- Init Map Data
    width = 25;
    height = 25;
    spacing = 2;

    // --- Create Map Generator
    mapGenerator = new MapGenerator(width, height, cellSize, DynArray<Tile*>()); //*** shared array of tiles?

    //// --- Create Tile Manager
    //tileManager = new TileManager();

    // --- Create Scenes
    sceneMapGenerator = new SceneMapGenerator();
    sceneMapGeneratorUI = new SceneMapGeneratorUI(sceneMapGenerator, DynArray<unsigned int>());
    sceneTileManagerUI = new SceneTileManagerUI();

    // --- Events
    App->event->Subscribe(this, &MainScene::OnPlay);
    App->event->Subscribe(this, &MainScene::OnStep);
    App->event->Subscribe(this, &MainScene::OnStop);
    App->event->Subscribe(this, &MainScene::OnSetCell);
    App->event->Subscribe(this, &MainScene::OnPresetCells);
    App->event->Subscribe(this, &MainScene::OnResetCells);
    App->event->Subscribe(this, &MainScene::OnResetAllCells);
    App->event->Subscribe(this, &MainScene::OnMapResize);
    App->event->Subscribe(this, &MainScene::OnDrawSpaced);
    App->event->Subscribe(this, &MainScene::OnChangeScene);

    return true;
}

bool MainScene::Start()
{
    // --- Init Scenes
    sceneMapGenerator->Init();
    sceneMapGeneratorUI->Init();
    sceneTileManagerUI->Init();

    // --- Start Scenes
    sceneMapGenerator->Start(width, height, cellSize, spacing, sceneMapGeneratorUI->GetPanelX(), sceneMapGeneratorUI->GetPanelY(), sceneMapGeneratorUI->GetPanelWidth(), sceneMapGeneratorUI->GetPanelHeight());
    sceneMapGeneratorUI->Start();
    sceneTileManagerUI->Start();

    // Set Scene Camera (created in sceneMapGenerator->Init())
    this->camera = sceneMapGenerator->GetCamera();

    return true;
}

bool MainScene::Update(float dt)
{
    if (isMapVisible)
    {
        if (mapGenerator->IsFinished())
            App->event->Publish(new EventFinish());

        // Update Scenes
        sceneMapGenerator->PreUpdate(dt);
        sceneMapGenerator->Update(dt);
        //sceneMapGenerator->PostUpdate(dt);
    }
    else
    {
        // Update Scenes
        //sceneTileManagerUI->PreUpdate(dt);
        //sceneTileManagerUI->Update(dt);
        //sceneTileManagerUI->PostUpdate(dt);
    }
    return true;
}

bool MainScene::CleanUp()
{
    sceneMapGenerator->CleanUp();
    sceneMapGeneratorUI->CleanUp();
    sceneTileManagerUI->CleanUp();

    delete sceneMapGenerator;
    delete sceneMapGeneratorUI;
    delete sceneTileManagerUI;

    delete mapGenerator;
    //delete tileManager;

    return true;
}

bool MainScene::Draw()
{
    // --- Map Generator
    if (isMapVisible)
    {
        DrawMap();
        sceneMapGenerator->Draw();
    }

    return true;
}

bool MainScene::DrawUI()
{
    // --- Map Generator
    if (isMapVisible)
    {
        Cell* cell = nullptr;
        if (!sceneMapGenerator->GetSelected().empty())
        {
            int index = sceneMapGenerator->GetSelected().front();
            cell = mapGenerator->GetCell(index);
        }

        sceneMapGeneratorUI->Draw(width, height, cell);
        return true;
    }

    // --- Tile Manager
    sceneTileManagerUI->Draw();
    return true;
}

void MainScene::DrawMap()
{
    static const Color black = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const Color gray = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const Color red = { 1.0f, 0.0f, 0.0f, 1.0f };
    static const Color green = { 0.0f, 1.0f, 0.0f, 1.0f };

    // Offset
    int offsetX = ((int)App->window->GetWidth() - ((width + spacing) * cellSize)) / 2;
    int offsetY = ((int)App->window->GetHeight() - ((height + spacing) * cellSize)) / 2;

    // --- Draw Map ---
    int numCells = width * height;
    for (unsigned int i = 0; i < numCells; ++i)
    {
        // Cell
        Cell* cell = mapGenerator->GetCell(i);
        int x = cell->index % width;
        int y = cell->index / width;

        // Position & Size
        glm::vec2 position = { offsetX + x * (cellSize + spacing), offsetY + y * (cellSize + spacing) };
        glm::vec2 size = { cellSize, cellSize };

        // Draw Texture
        if (sceneMapGeneratorUI->IsDrawTextures() && !cell->isInvalid && (cell->isCollapsed || cell->isPreset))
        {
            Tile* tile = mapGenerator->GetTile(cell->tileID);
            App->renderer->DrawQuad(position, size, tile->GetTexture());
            continue;
        }

        // Draw Color
        Color color = (cell->isInvalid) ? red : gray;
        if (!sceneMapGeneratorUI->IsDrawTextures() && cell->isCollapsed && cell->tileID != 0)
            color = black;

        App->renderer->DrawQuad(position, size, glm::vec4(color.r, color.g, color.b, color.a));
    }
}

// -------------------------------
// --- EVENTS ---
void MainScene::OnPlay(EventPlay* e)
{
    timer.Start();

    mapGenerator->GenerateMap();

    sceneMapGeneratorUI->AddTime(timer.ReadMs());
}

void MainScene::OnStep(EventStep* e)
{
    timer.Start();

    mapGenerator->Step();

    float time = timer.ReadMs();
    sceneMapGeneratorUI->SetStepTime(time);
    sceneMapGeneratorUI->AddTime(time);
}

void MainScene::OnStop(EventStop* e)
{
    mapGenerator->ResetMap();

    sceneMapGeneratorUI->SetStepTime(0.0f);
    sceneMapGeneratorUI->SetTotalTime(0.0f);
}

void MainScene::OnSetCell(EventSetCell* e)
{
    mapGenerator->SetCell(e->cell, e->tileID);
}

void MainScene::OnPresetCells(EventPresetCells* e)
{
    for (unsigned int i = 0; i < e->cells.size(); ++i)
    {
        int index = e->cells.at(i);
        mapGenerator->PresetCell(index, e->tileID);
    }
}

void MainScene::OnResetCells(EventResetCells* e)
{
    for (unsigned int i = 0; i < e->cells.size(); ++i)
    {
        int index = e->cells.at(i);
        mapGenerator->ResetCell(index);
    }
}

void MainScene::OnResetAllCells(EventResetAllCells* e)
{
    mapGenerator->ClearPresetCells();
}

void MainScene::OnMapResize(EventMapResize* e)
{
    width = e->width;
    height = e->height;

    mapGenerator->SetSize(e->width, e->height);
}

void MainScene::OnDrawSpaced(EventDrawSpaced* e)
{
    static const int defaultSpacing = 2;
    spacing = (e->value) ? defaultSpacing : 0;

    App->event->Publish(new EventSpacingChange(spacing));
}

void MainScene::OnChangeScene(EventChangeScene* e)
{
    if (e->scene == "MapGenerator")
    {
        isMapVisible = true;
    }
    else if (e->scene == "TileManager")
    {
        isMapVisible = false;
    }
}
