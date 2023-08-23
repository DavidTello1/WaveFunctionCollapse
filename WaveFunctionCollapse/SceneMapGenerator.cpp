#include "SceneMapGenerator.h"
#include "Camera.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"

#include "MapGenerator.h"
#include "Tile.h"
#include "Cell.h"
#include "ButtonGrid.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "SDL/include/SDL_scancode.h" //***

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
	isPlay = false;
	isDrawTextures = true;
	isDrawSpaced = true;

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

	// --- Button Grid --- 
	buttonGrid = new ButtonGrid(0, 0, width, height, cellSize, 0, ButtonGrid::Type::MULTIPLE_SELECTION);
	UpdateButtonsPosition();

	return true;
}

bool SceneMapGenerator::Update(float dt)
{
	// --- Update Button Grid
	buttonGrid->Update();

	// -------------------
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) // Step
	{
		if (map->IsFinished())
			map->ResetMap();

		if (isPlay == false)
			buttonGrid->UnSelectAll();

		map->Step();
		isPlay = true;
		buttonGrid->SetSelectionType(ButtonGrid::Type::SINGLE_SELECTION);

	}
	else if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) // Play
	{
		if (map->IsFinished())
			map->ResetMap();

		map->GenerateMap();
		isPlay = true;
		buttonGrid->SetSelectionType(ButtonGrid::Type::SINGLE_SELECTION);
		buttonGrid->UnSelectAll();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) // Stop
	{
		map->ResetMap();
		isPlay = false;
		buttonGrid->SetSelectionType(ButtonGrid::Type::MULTIPLE_SELECTION);
		buttonGrid->UnSelectAll();
	}

	return true;
}

bool SceneMapGenerator::CleanUp()
{
	delete camera;

	delete buttonGrid;
	delete map;

    return true;
}

bool SceneMapGenerator::Draw()
{
	static const glm::vec3 black = { 0, 0, 0 };
	static const glm::vec3 gray = { 0.5f, 0.5f, 0.5f };
	static const glm::vec3 red = { 255, 0, 0 };
	static const glm::vec3 green = { 0, 255, 0 };

	// --- Draw Map ---
	int numCells = width * height;
	for (unsigned int i = 0; i < numCells; ++i)
	{
		Cell* cell = map->GetCell(i);
		int x = cell->index % width;
		int y = cell->index / width;

		int padding = 0;
		if (isDrawSpaced)
			padding = spacing;
			
		int offsetX = (App->window->GetWidth() - ((width + padding) * cellSize)) / 2;
		int offsetY = (App->window->GetHeight() - ((height + padding) * cellSize)) / 2;

		glm::vec2 position = { offsetX + x * (cellSize + padding), offsetY + y * (cellSize + padding)};
		glm::vec2 size = { cellSize, cellSize };

		if (isDrawTextures)
		{
			if (!cell->isInvalid && (cell->isCollapsed || cell->tileID != -1))
			{
				Tile* tile = map->GetTile(cell->tileID);
				App->renderer->DrawQuad(position, size, tile->texture);
			}
			else
			{
				glm::vec3 color = (cell->isInvalid) ? red : gray;
				App->renderer->DrawQuad(position, size, glm::vec4(color, 1.0f));
			}
		}
		else
		{
			glm::vec3 color = gray;
			if (cell->isInvalid)
				color = red;
			else if (cell->isCollapsed && cell->tileID != 0)
				color = black;

			App->renderer->DrawQuad(position, size, glm::vec4(color, 1.0f));
		}
	}

	return true;
}

bool SceneMapGenerator::DrawUI()
{
	DrawPanel();
	buttonGrid->Draw();

	return true;
}

void SceneMapGenerator::PresetCells(const List<unsigned int>& cells, const unsigned int tileID)
{
	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		int index = cells[i]->data;
		map->PresetCell(index, tileID);
	}

	buttonGrid->UnSelectAll();
}

void SceneMapGenerator::ClearCells(const List<unsigned int>& cells)
{
	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		int index = cells[i]->data;
		map->ResetCell(index);
	}

	buttonGrid->UnSelectAll();
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
		static const int buttonSize = 30;
		static const int paddingX = (ImGui::GetContentRegionAvailWidth() - 3 * (buttonSize + 12)) / 2;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + paddingX);
		ImGui::ImageButton((ImTextureID)stepIcon, ImVec2(buttonSize, buttonSize));
		ImGui::SameLine();
		ImGui::ImageButton((ImTextureID)playIcon, ImVec2(buttonSize, buttonSize));
		ImGui::SameLine();
		ImGui::ImageButton((ImTextureID)restartIcon, ImVec2(buttonSize, buttonSize));
		ImGui::Separator();

		// --- World Data
		int size[2] = { width, height };
		ImGui::DragInt2("Size", size, 1.0f, 1, 1000);

		ImGui::Checkbox("Draw Textures", &isDrawTextures);
		if (ImGui::Checkbox("Draw Spacing", &isDrawSpaced)) {
			UpdateButtonsPosition();
		}
		ImGui::Separator();

		// --- Cell Inspection
		DrawCellInspector();
	}
	ImGui::End();
}

void SceneMapGenerator::DrawCellInspector()
{
	List<unsigned int> tileArray;

	if (isPlay)
	{
		if (buttonGrid->GetSelected().empty())
			return;

		int index = buttonGrid->GetSelected().front()->data;
		Cell* cell = map->GetCell(index);
		ImTextureID texture = 0;
		if (cell->isCollapsed)
			texture = (ImTextureID)map->GetTile(cell->tileID)->texture;

		ImGui::Columns(2, "Cell Inspection", false);
		ImGui::SetColumnWidth(0, 65);
		ImGui::Image(texture, ImVec2(50, 50));
		ImGui::NextColumn();

		ImGui::Text("Index: %d", index);
		ImGui::Text("Position: %d,%d", index % width, index / width);
		ImGui::Text("Collapsed: %s", (cell->isCollapsed) ? "true" : "false");
		ImGui::Text("TileID: %d", cell->tileID);
		ImGui::Text("Mask: %s", cell->mask->ToString());
		ImGui::Columns(1);

		tileArray = cell->mask->GetSetBits();
	}
	else
	{
		ImGui::Text("Selected Cells: %d", buttonGrid->GetSelected().size());

		if (buttonGrid->GetSelected().empty())
			return;

		if (ImGui::Button("Clear Cells"))
		{
			ClearCells(buttonGrid->GetSelected());
		}

		for (unsigned int i = 0; i < numTiles; ++i)
			tileArray.add(i);
	}

	// --- Tiles Selector
	static const int imageWidth = 30;
	static const int numColumns = 4;
	static const int menuBarHeight = 32;
	static const int itemSpacing = 10;

	int numRows = tileArray.size() / numColumns;
	if (tileArray.size() % numColumns != 0)
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
		for (int i = 0; i < tileArray.size(); ++i)
		{
			unsigned int index = tileArray[i]->data;
			ImTextureID texture = (ImTextureID)map->GetTile(index)->texture;
			if (ImGui::ImageButton(texture, ImVec2(imageWidth, imageWidth)))
			{
				if (isPlay)
				{
					map->SetCell(buttonGrid->GetSelected().front()->data, index);
					buttonGrid->UnSelectAll();
				}
				else
					PresetCells(buttonGrid->GetSelected(), index);
			}
			count++;

			if (count < numColumns)
				ImGui::SameLine();
			else
				count = 0;
		}
	}
	ImGui::EndChild();
}

void SceneMapGenerator::UpdateButtonsPosition()
{
	int padding = 0;
	if (isDrawSpaced)
		padding = spacing;

	int x = (App->window->GetWidth() - ((width + padding) * cellSize)) / 2;
	int y = (App->window->GetHeight() - ((height + padding) * cellSize)) / 2;

	buttonGrid->SetPosition(x, y);
	buttonGrid->SetSpacing(padding);
}