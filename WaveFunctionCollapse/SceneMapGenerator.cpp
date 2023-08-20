#include "SceneMapGenerator.h"
#include "Camera.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleInput.h"

#include "MapGenerator.h"
#include "Tile.h"

#include "SDL/include/SDL_scancode.h"

#include "mmgr/mmgr.h"

SceneMapGenerator::SceneMapGenerator() : Scene("MapGenerator")
{
}

SceneMapGenerator::~SceneMapGenerator()
{
}

bool SceneMapGenerator::Init()
{
	width = 25;
	height = 25;

	camera = new Camera();

    return true;
}

bool SceneMapGenerator::Start()
{
	// --- Tiles																					 // TL		// Top	   // TR	  // Left	 // Right	// BL	   // Bottom  // BR
	Tile* empty =		new Tile(0, App->resources->LoadTexture("Assets/Textures/empty.png")->index,		"1111111", "1001111", "1111111", "1010111", "1101011", "1111111", "1110011", "1111111");
	Tile* topLeft =		new Tile(1, App->resources->LoadTexture("Assets/Textures/topLeft.png")->index,		"1000000", "1000000", "1100001", "1000000", "0010110", "1100010", "0001101", "1000100");
	Tile* topRight =	new Tile(2, App->resources->LoadTexture("Assets/Textures/topRight.png")->index,	"1010001", "1000000", "1000000", "0101010", "1000000", "1001000", "0001101", "1010010");
	Tile* bottomLeft =	new Tile(3, App->resources->LoadTexture("Assets/Textures/bottomLeft.png")->index,  "1001010", "0110001", "1010000", "1000000", "0010110", "1000000", "1000000", "1001001");
	Tile* bottomRight =	new Tile(4, App->resources->LoadTexture("Assets/Textures/bottomRight.png")->index, "1100000", "0110001", "1000110", "0101010", "1000000", "1000101", "1000000", "1000000");
	Tile* horizontal =	new Tile(5, App->resources->LoadTexture("Assets/Textures/horizontal.png")->index,	"1010001", "1000000", "1100001", "1101010", "1010110", "1000101", "1000000", "1001001");
	Tile* vertical =	new Tile(6, App->resources->LoadTexture("Assets/Textures/vertical.png")->index,	"1001010", "1110001", "1000110", "1000000", "1000000", "1100010", "1001101", "1010010");
	
	static const int numTiles = 7;
	DynArray<Tile*> tiles = DynArray<Tile*>(numTiles);
	tiles.push_back(empty);
	tiles.push_back(topLeft);
	tiles.push_back(topRight);
	tiles.push_back(bottomLeft);
	tiles.push_back(bottomRight);
	tiles.push_back(horizontal);
	tiles.push_back(vertical);

	// --- Create Map Generator ---
	map = new MapGenerator(width, height, cellSize, tiles);
	//map->GenerateMap();

	return true;
}

bool SceneMapGenerator::Update(float dt)
{
	static bool isDrawTextures = true;
	static bool isSpacedCells = true;

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		map->IsFinished() ? map->Reset() : map->Step();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		if (map->IsFinished())
			map->Reset();

		map->GenerateMap();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		isDrawTextures = !isDrawTextures;
		map->SetDrawTextures(isDrawTextures);
	}
	else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		isSpacedCells = !isSpacedCells;
		map->SetSpacedCells(isSpacedCells);
	}

	return true;
}

bool SceneMapGenerator::CleanUp()
{
	delete camera;
	delete map;

    return true;
}

bool SceneMapGenerator::Draw()
{
	if (map != nullptr)
		map->DrawMap();

	return true;
}

bool SceneMapGenerator::DrawUI()
{

	return true;
}