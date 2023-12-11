#include "SceneTileManager.h"
#include "Tile.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleWindow.h"
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
			App->scene->SetCurrentScene("MapGenerator");
		}
	}
	ImGui::EndMainMenuBar();
}

void SceneTileManager::DrawToolbar()
{
	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoTitleBar;

	static const int toolbarX = 0;
	static const int toolbarY = menubarHeight;
	static const int toolbarHeight = 46;

	static const int buttonSize = 24;
	
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth(), toolbarHeight), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(toolbarX, toolbarY), ImGuiCond_Once);

	if (ImGui::Begin("Toolbar", NULL, flags))
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
	}
	ImGui::End();

}

void SceneTileManager::DrawHierarchy()
{
	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

	static const int hierarchyX = 0;
	static const int hierarchyY = menubarHeight + toolbarHeight;
	int hierarchyHeight = App->window->GetHeight() - hierarchyY;

	ImGui::SetNextWindowPos(ImVec2(hierarchyX, hierarchyY), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(hierarchyWidth, hierarchyHeight), ImGuiCond_Always);

	if (ImGui::Begin("Hierarchy", NULL, flags))
	{
		for (unsigned int i = 0; i < tiles.size(); ++i)
		{
			String name = String("Tile %d", i);
			if (ImGui::Selectable(name.c_str(), currentTile == i, ImGuiSelectableFlags_SpanAvailWidth))
				currentTile = i;
		}
	}
	ImGui::End();
}

void SceneTileManager::DrawMainPanel()
{
	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

	static const int mainPanelX = hierarchyWidth;
	static const int mainPanelY = menubarHeight + toolbarHeight;
	int mainPanelWidth = App->window->GetWidth() - mainPanelX;
	int mainPanelHeight = App->window->GetHeight() - mainPanelY;

	ImGui::SetNextWindowPos(ImVec2(mainPanelX, mainPanelY), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mainPanelWidth, mainPanelHeight), ImGuiCond_Always);

	if (ImGui::Begin("Tile", NULL, flags))
	{
		DrawNeighbours(); 

		ImGui::SameLine();
		DrawTileInfo();

		DrawTileList();
	}
	ImGui::End();
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
	}
	ImGui::EndChild();
}


void SceneTileManager::DrawTileInfo()
{
	static const int childSize = 120+16;

	if (ImGui::BeginChild("Tile Info", ImVec2(0, childSize)))
	{
		ImGui::Text("Name: ");
		ImGui::Text("Path: ");

	}
	ImGui::EndChild();
}

void SceneTileManager::DrawTileList()
{
	if (ImGui::BeginChild("Tile List", ImVec2(0, 0), true))
	{

	}
	ImGui::EndChild();
}
