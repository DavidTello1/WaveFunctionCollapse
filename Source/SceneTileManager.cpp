#include "SceneTileManager.h"
#include "Tile.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleScene.h"

#include "String.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

SceneTileManager::SceneTileManager() : Scene("TileManager")
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

bool SceneTileManager::DrawUI()
{
	DrawMenuBar();
	DrawToolbar();
	DrawHierarchy();
	DrawMainPanel();

	return true;
}

// -------------------------------
// --- UI PANELS ---
void SceneTileManager::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import", "Ctrl+O"))
			{
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save Tile", "Ctrl+S"))
			{
			}
			if (ImGui::MenuItem("Save All", "Ctrl+Shift+S"))
			{
			}
			if (ImGui::MenuItem("Save As"))
			{
			}

			ImGui::EndMenu();
		}

		ImGui::Text("|");
		if (ImGui::MenuItem("Map Generator"))
		{
			//*** change to SceneMapGenerator
		}
	}
	ImGui::EndMainMenuBar();
}

void SceneTileManager::DrawToolbar()
{
	static const int buttonSize = 24;

	if (ImGui::Begin("Toolbar", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::ImageButton(NULL, ImVec2(buttonSize, buttonSize))) // Save
		{
			//*** SAVE CURRENT TILE
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(NULL, ImVec2(buttonSize, buttonSize))) // Save All
		{
			//*** SAVE ALL TILES
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(NULL, ImVec2(buttonSize, buttonSize))) // Import
		{
			//*** IMPORT TILE
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(NULL, ImVec2(buttonSize, buttonSize))) // Filter
		{
			//*** FILTER NEIGHBOURS
		}

		ImGui::End();
	}

}

void SceneTileManager::DrawHierarchy()
{
	if (ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse))
	{
		for (unsigned int i = 0; i < tiles.size(); ++i)
		{
			String name = String("Tile %d", i);
			if (ImGui::Selectable(name.c_str(), currentTile == i, ImGuiSelectableFlags_SpanAvailWidth))
				currentTile = i;
		}

		ImGui::End();
	}
}

void SceneTileManager::DrawMainPanel()
{
	if (ImGui::Begin("Tile", NULL, ImGuiWindowFlags_NoCollapse))
	{
		DrawNeighbours(); 

		ImGui::SameLine();
		DrawTileInfo();

		DrawTileList();

		ImGui::End();
	}
}

void SceneTileManager::DrawNeighbours()
{
	static const int size = 40;
	static const int childSize = size * 3 + 16;

	if (ImGui::BeginChild("Neighbours", ImVec2(childSize, childSize)))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1, 0, 0, 1));

		ImGui::Dummy(ImVec2(size, size));
		ImGui::SameLine();
		if (ImGui::Selectable("###Top", true/*currentDir == 0*/, ImGuiSelectableFlags_None, ImVec2(size, size)))
			currentDir = 0;

		if (ImGui::Selectable("###Left", true/*currentDir == 1*/, ImGuiSelectableFlags_None, ImVec2(size, size)))
			currentDir = 1;

		ImGui::SameLine();
		ImTextureID tileTexture = 0;
		if (!tiles.empty())
			tileTexture = (ImTextureID)tiles[currentTile]->GetTexture();
		ImGui::Image(tileTexture, ImVec2(size, size));
		ImGui::SameLine();

		if (ImGui::Selectable("###Right", true/*currentDir == 2*/, ImGuiSelectableFlags_None, ImVec2(size, size)))
			currentDir = 2;

		ImGui::Dummy(ImVec2(size, size));
		ImGui::SameLine();
		if (ImGui::Selectable("###Bottom", true/*currentDir == 3*/, ImGuiSelectableFlags_None, ImVec2(size, size)))
			currentDir = 3;

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::EndChild();
	}
}


void SceneTileManager::DrawTileInfo()
{
	static const int childSize = 120+16;

	if (ImGui::BeginChild("Tile Info", ImVec2(0, childSize)))
	{
		ImGui::Text("Name: ");
		ImGui::Text("Path: ");

		ImGui::EndChild();
	}
}

void SceneTileManager::DrawTileList()
{
	if (ImGui::BeginChild("Tile List", ImVec2(0, 0), true))
	{

		ImGui::EndChild();
	}
}
