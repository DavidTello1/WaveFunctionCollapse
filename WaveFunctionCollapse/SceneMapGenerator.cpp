#include "SceneMapGenerator.h"
#include "Camera.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"

#include "MapGenerator.h"
#include "Tile.h"
#include "UI_Button.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
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

	// --- Buttons
	glm::vec4 idleColor = { 0, 0, 0, 0 };
	glm::vec4 hoverColor = { 0, 0, 1, 0.5f };
	glm::vec4 selectedColor = { 0, 0, 1, 0.75f };

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			buttons.add(new UI_Button(x, y, cellSize, cellSize, idleColor, hoverColor, selectedColor));
		}
	}
	UpdateButtonsPosition();

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
	// --- UI Buttons
	ListItem<UI_Button*>* item;
	for (item = buttons.front(); item != nullptr; item = item->next)
	{
		item->data->Update();
	}

	// -------------------
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

	return true;
}

bool SceneMapGenerator::CleanUp()
{
	delete camera;
	delete map;

	ListItem<UI_Button*>* item;
	for (item = buttons.front(); item != nullptr; item = item->next)
	{
		delete item->data;
	}
	buttons.clear();

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
	DrawPanel();
	DrawButtons();

	return true;
}

void SceneMapGenerator::DrawPanel()
{
	static const int panelWidth = 220;
	static const int panelHeight = App->window->GetHeight() - 6;
	static const int panelX = App->window->GetWidth() - panelWidth - 3;
	static const int panelY = 3;

	ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(panelX, panelY), ImGuiCond_Once);

	if (ImGui::Begin("Options", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		// --- Buttons
		ImGui::ImageButton((ImTextureID)stepIcon, ImVec2(30, 30));
		ImGui::SameLine();
		ImGui::ImageButton((ImTextureID)playIcon, ImVec2(30, 30));
		ImGui::SameLine();
		ImGui::ImageButton((ImTextureID)restartIcon, ImVec2(30, 30));
		ImGui::Separator();

		// --- World Data
		int size[2] = { width, height };
		ImGui::DragInt2("Size", size, 1.0f, 1, 1000);

		if (ImGui::Checkbox("Draw Textures", &isDrawTextures)) {
			map->SetDrawTextures(isDrawTextures);
		}
		if (ImGui::Checkbox("Draw Spacing", &isDrawSpaced)) {
			map->SetSpacedCells(isDrawSpaced);
			UpdateButtonsPosition();
		}
		ImGui::Separator();

		// --- Cell Inspection
		ImGui::Columns(2, "Cell Inspection", false);
		ImGui::SetColumnWidth(0, 65);
		ImGui::Image((ImTextureID)0, ImVec2(50, 50));
		ImGui::NextColumn();

		ImGui::Text("Index: %d", 0);
		ImGui::Text("Position: %d,%d", 0, 0);
		ImGui::Text("isCollapsed: %s", false);
		ImGui::Text("tileID: %d", -1);
		ImGui::Text("mask: %s", "1111111");
		ImGui::Columns(1);

		// --- Tiles Selector
		static const int imageWidth = 30;
		static const int numColumns = 4;
		static const int menuBarHeight = 32;
		static const int itemSpacing = 10;

		int numRows = numTiles / numColumns;
		if (numTiles % numColumns != 0)
			numRows++;

		int childHeight = numRows * (imageWidth + itemSpacing) + menuBarHeight;
		childHeight = MIN(childHeight, ImGui::GetContentRegionAvail().y);
		if (ImGui::BeginChild("Mask Inspector", ImVec2(0, childHeight), true, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::EndMenuBar();
			}

			int count = 0;
			for (int i = 0; i < numTiles; ++i)
			{
				ImGui::ImageButton((ImTextureID)0, ImVec2(imageWidth, imageWidth));
				count++;

				if (count < numColumns)
					ImGui::SameLine();
				else
					count = 0;
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void SceneMapGenerator::DrawButtons()
{
	int numCells = width * height;
	for (int i = 0; i < numCells; ++i)
	{
		buttons[i]->data->Draw();
	}
}

void SceneMapGenerator::UpdateButtonsPosition()
{
	static const int offsetX = (App->window->GetWidth() - (width * cellSize)) / 2;
	static const int offsetY = (App->window->GetHeight() - (height * cellSize)) / 2;

	static const int spacing = 2;

	int numCells = width * height;
	for (int i = 0; i < numCells; ++i)
	{
		int x = i % width;
		int y = i / width;

		glm::vec2 position = {x * cellSize + offsetX, y * cellSize + offsetY };
		glm::vec2 size = { cellSize, cellSize };

		if (isDrawSpaced)
		{
			const int offsetSpacedX = (App->window->GetWidth() - ((width + spacing) * cellSize)) / 2;
			const int offsetSpacedY = (App->window->GetHeight() - ((height + spacing) * cellSize)) / 2;

			position.x = x * cellSize + offsetSpacedX + x * spacing;
			position.y = y * cellSize + offsetSpacedY + y * spacing;
		}

		buttons[i]->data->SetPos(position.x, position.y);
	}
}