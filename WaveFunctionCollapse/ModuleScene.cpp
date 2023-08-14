#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleInput.h"

#include "MapGenerator.h"
#include "Tile.h"

#include "SDL2/include/SDL_scancode.h"

#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	return true;
}

bool ModuleScene::Start()
{
	// --- Create Tiles ---
	static const int numTiles = 7;

	Tile* empty = new Tile(0, App->resources->LoadTexture("Textures/empty.png"), "1001111", "1010111", "1101011", "1110011");
	Tile* topLeft = new Tile(1, App->resources->LoadTexture("Textures/topLeft.png"), "1000000", "1000000", "0010110", "0001101");
	Tile* topRight = new Tile(2, App->resources->LoadTexture("Textures/topRight.png"), "1000000", "0101010", "1000000", "0001101");
	Tile* bottomLeft = new Tile(3, App->resources->LoadTexture("Textures/bottomLeft.png"), "0110001", "1000000", "0010110", "1000000");
	Tile* bottomRight = new Tile(4, App->resources->LoadTexture("Textures/bottomRight.png"), "0110001", "0101010", "1000000", "1000000");
	Tile* horizontal = new Tile(5, App->resources->LoadTexture("Textures/horizontal.png"), "1000000", "1101010", "1010110", "1000000");
	Tile* vertical = new Tile(6, App->resources->LoadTexture("Textures/vertical.png"), "1110001", "1000000", "1000000", "1001101");

	DynArray<Tile*> tiles = DynArray<Tile*>(numTiles);
	tiles.push_back(empty);
	tiles.push_back(topLeft);
	tiles.push_back(topRight);
	tiles.push_back(bottomLeft);
	tiles.push_back(bottomRight);
	tiles.push_back(horizontal);
	tiles.push_back(vertical);

	// --- Create Map Generator ---
	map = new MapGenerator(25, 25, 30, tiles);

	return true;
}

bool ModuleScene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		map->IsFinished() ? map->Reset() : map->Step();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		map->IsFinished() ? map->Reset() : map->GenerateMap();
	}

	if (map != nullptr)
		map->DrawMap();

	return true;
}

bool ModuleScene::CleanUp()
{
	delete map;

	return true;
}
