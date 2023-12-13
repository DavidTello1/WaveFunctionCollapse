#include "SceneTileManager.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleResources.h"

#include "Tile.h"

#include "mmgr/mmgr.h"

SceneTileManager::SceneTileManager()
{
}

SceneTileManager::~SceneTileManager()
{
}

bool SceneTileManager::Init()
{
	// -----------------------------------------
	Tile* empty = new Tile(0, App->resources->LoadTexture("Assets/Textures/empty.png")->index, "1001111", "1010111", "1101011", "1110011");
	Tile* topLeft = new Tile(1, App->resources->LoadTexture("Assets/Textures/topLeft.png")->index, "1000000", "1000000", "0010110", "0001101");
	Tile* topRight = new Tile(2, App->resources->LoadTexture("Assets/Textures/topRight.png")->index, "1000000", "0101010", "1000000", "0001101");
	Tile* bottomLeft = new Tile(3, App->resources->LoadTexture("Assets/Textures/bottomLeft.png")->index, "0110001", "1000000", "0010110", "1000000");
	Tile* bottomRight = new Tile(4, App->resources->LoadTexture("Assets/Textures/bottomRight.png")->index, "0110001", "0101010", "1000000", "1000000");
	Tile* horizontal = new Tile(5, App->resources->LoadTexture("Assets/Textures/horizontal.png")->index, "1000000", "1101010", "1010110", "1000000");
	Tile* vertical = new Tile(6, App->resources->LoadTexture("Assets/Textures/vertical.png")->index, "1110001", "1000000", "1000000", "1001101");

	tiles.push_back(empty);
	tiles.push_back(topLeft);
	tiles.push_back(topRight);
	tiles.push_back(bottomLeft);
	tiles.push_back(bottomRight);
	tiles.push_back(horizontal);
	tiles.push_back(vertical);
	//----------------------------------------

	return true;
}

bool SceneTileManager::Start()
{
	// --- Test ---
	for (unsigned int i = 0; i < tiles.size(); ++i)
	{
		Tile* tile = tiles[i];
		App->event->Publish(new EventImportTile(tile));
	}
	// ---

	return true;
}

bool SceneTileManager::Update(float dt)
{
	return true;
}

bool SceneTileManager::CleanUp()
{
	for (unsigned int i = 0; i < tiles.size(); ++i)
	{
		delete tiles[i];
	}
	tiles.clear();

	return true;
}
