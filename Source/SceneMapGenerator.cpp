#include "SceneMapGenerator.h"
#include "Camera.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleResources.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"
#include "ModuleScene.h"

#include "MapGenerator.h"
#include "Tile.h"
#include "Cell.h"

#include "CameraController.h"

#include "ButtonGrid.h"
#include "UI_Button.h"

#include "PerfTimer.h"

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
	widthRatio = 1.0f;
	heightRatio = 1.0f;

	isMapPreset = false;
	isDrawTextures = true;
	isDrawSpaced = true;

	isPanelOpen = true;
	panelWidth = 220;
	panelHeight = App->window->GetHeight() - menuBarHeight;
	panelX = App->window->GetWidth() - panelWidth;

	camera = new Camera(glm::vec3(0.0f), 0.0f, 1.0f);
	controller = new CameraController(camera, 0.3f);

	state = State::STOP;

	// --- Events
	App->event->Subscribe(this, &SceneMapGenerator::OnResize);
	App->event->Subscribe(this, &SceneMapGenerator::OnZoom);

    return true;
}

bool SceneMapGenerator::Start()
{
	// --- Icons (UI)
	playIcon = App->resources->LoadTexture("Assets/Textures/Icons/play.png")->index;
	stepIcon = App->resources->LoadTexture("Assets/Textures/Icons/step.png")->index;
	stopIcon = App->resources->LoadTexture("Assets/Textures/Icons/stop.png")->index;
	restartIcon = App->resources->LoadTexture("Assets/Textures/Icons/restart.png")->index;

	// --- Tiles																							// Top	   // Left	 // Right	// Bottom
	Tile* empty =		new Tile(0, App->resources->LoadTexture("Assets/Textures/empty.png")->index,		"1001111", "1010111", "1101011","1110011");
	Tile* topLeft =		new Tile(1, App->resources->LoadTexture("Assets/Textures/topLeft.png")->index,		"1000000", "1000000", "0010110","0001101");
	Tile* topRight =	new Tile(2, App->resources->LoadTexture("Assets/Textures/topRight.png")->index,		"1000000", "0101010", "1000000","0001101");
	Tile* bottomLeft =	new Tile(3, App->resources->LoadTexture("Assets/Textures/bottomLeft.png")->index,	"0110001", "1000000", "0010110","1000000");
	Tile* bottomRight =	new Tile(4, App->resources->LoadTexture("Assets/Textures/bottomRight.png")->index,	"0110001", "0101010", "1000000","1000000");
	Tile* horizontal =	new Tile(5, App->resources->LoadTexture("Assets/Textures/horizontal.png")->index,	"1000000", "1101010", "1010110","1000000");
	Tile* vertical =	new Tile(6, App->resources->LoadTexture("Assets/Textures/vertical.png")->index,		"1110001", "1000000", "1000000","1001101");
	
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

	// --- Button Grid --- 
	buttonGrid = new ButtonGrid(0, 0, width, height, cellSize, spacing, ButtonGrid::Type::MULTIPLE_SELECTION);
	UpdateButtonGrid();

	// --- Background Button ---
	Color transparent = { 0,0,0,0 };
	bgButton = new UI_Button(0, menuBarHeight, panelX, App->window->GetHeight(), transparent, transparent, transparent, true);

	// --- Block Button for Panel ---
	blockButton = new UI_Button(panelX, menuBarHeight, panelWidth, panelHeight, transparent, transparent, transparent, true);

	return true;
}

bool SceneMapGenerator::Update(float dt)
{
	// --- Check if map is finished
	if (map->IsFinished())
		state = State::FINISHED;

	// --- Update Scene Elements
	controller->Update(dt);

	if (!blockButton->IsHovered())
		buttonGrid->Update(dt);

	bgButton->Update(dt);
	if (!buttonGrid->IsHovered() && bgButton->IsClicked() &&
		App->input->GetKey(SDL_SCANCODE_RCTRL) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT &&
		App->input->GetKey(SDL_SCANCODE_RALT) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
	{
		buttonGrid->UnSelectAll();
	}

	// --- Shortcuts
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) // Step
	{
		Step();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) // Play
	{
		Restart();
		Play();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) // Stop
	{
		Stop();
	}

	return true;
}

bool SceneMapGenerator::CleanUp()
{
	delete camera;
	delete controller;

	delete buttonGrid;
	delete bgButton;
	delete blockButton;

	delete map;

    return true;
}

bool SceneMapGenerator::Draw()
{
	static const Color black = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const Color gray  = { 0.5f, 0.5f, 0.5f, 1.0f };
	static const Color red   = { 1.0f, 0.0f, 0.0f, 1.0f };
	static const Color green = { 0.0f, 1.0f, 0.0f, 1.0f };

	// --- Draw Map ---
	int numCells = width * height;
	for (unsigned int i = 0; i < numCells; ++i)
	{
		// Cell
		Cell* cell = map->GetCell(i);
		int x = cell->index % width;
		int y = cell->index / width;

		// Padding
		int padding = (isDrawSpaced) ? spacing : 0;

		// Offset
		int offsetX = ((int)App->window->GetWidth() - ((width + padding) * cellSize)) / 2;
		int offsetY = ((int)App->window->GetHeight() - ((height + padding) * cellSize)) / 2;

		// Position & Size
		glm::vec2 position = { offsetX + x * (cellSize + padding), offsetY + y * (cellSize + padding) };
		glm::vec2 size = { cellSize, cellSize };

		// Draw Texture
		if (isDrawTextures && !cell->isInvalid && (cell->isCollapsed || cell->isPreset))
		{
			Tile* tile = map->GetTile(cell->tileID);
			App->renderer->DrawQuad(position, size, tile->GetTexture());
			continue;
		}

		// Draw Color
		Color color = (cell->isInvalid) ? red : gray;
		if (!isDrawTextures && cell->isCollapsed && cell->tileID != 0)
			color = black;

		App->renderer->DrawQuad(position, size, glm::vec4(color.r, color.g, color.b, color.a));
	}

	return true;
}

bool SceneMapGenerator::DrawUI()
{
	buttonGrid->Draw();

	DrawMenuBar();
	DrawPanel();

	return true;
}

// -------------------------------
// --- DEBUG UTILS ---
void SceneMapGenerator::PresetCells(const List<unsigned int>& cells, const unsigned int tileID)
{
	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		int index = cells.at(i);
		map->PresetCell(index, tileID);
	}

	buttonGrid->UnSelectAll();
}

void SceneMapGenerator::ClearCells(const List<unsigned int>& cells)
{
	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		int index = cells.at(i);
		map->ResetCell(index);
	}

	buttonGrid->UnSelectAll();
}

void SceneMapGenerator::UpdateButtonGrid()
{
	int padding = (isDrawSpaced) ? spacing : 0;
	int offsetX = ((int)App->window->GetWidth() - ((width + padding) * cellSize)) / 2;
	int offsetY = ((int)App->window->GetHeight() - ((height + padding) * cellSize)) / 2;

	buttonGrid->SetPosX(offsetX);
	buttonGrid->SetPosY(offsetY);
	buttonGrid->SetSpacing(padding);
}

void SceneMapGenerator::MapResized(const int mapWidth, const int mapHeight)
{
	width = mapWidth;
	height = mapHeight;

	widthRatio = (float)width / (float)height;
	heightRatio = (float)height / (float)width;

	map->SetSize(mapWidth, mapHeight);

	buttonGrid->Resize(mapWidth, mapHeight);
	UpdateButtonGrid();
}

void SceneMapGenerator::PanelToggled(bool isOpen)
{
	panelWidth = (isOpen) ? 220 : 20;
	panelX = App->window->GetWidth() - panelWidth;

	bgButton->SetWidth(panelX);

	blockButton->SetWidth(panelWidth);
	blockButton->SetPosX(panelX);
}

String SceneMapGenerator::MaskToString(const BitArray& mask)
{
	String string = String(mask.size());

	for (unsigned int i = 0; i < mask.size(); ++i)
	{
		string += (mask.getBit(i)) ? '1' : '0';
	}

	return string;
}

// ------------------------
// --- STATE MANAGEMENT ---
void SceneMapGenerator::Play()
{
	timer.Start();

	if (state != State::STOP && state != State::PAUSE)
		return;

	state = State::PLAY;

	map->GenerateMap();

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(ButtonGrid::Type::SINGLE_SELECTION);

	totalTime += timer.ReadMs();
}

void SceneMapGenerator::Step()
{
	timer.Start();

	if (state != State::STOP && state != State::PAUSE)
		return;

	state = State::PAUSE;

	map->Step();

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(ButtonGrid::Type::SINGLE_SELECTION);

	stepTime = timer.ReadMs();
	totalTime += stepTime;
}

void SceneMapGenerator::Stop()
{
	if (state == State::STOP)
		return;

	state = State::STOP;

	map->ResetMap();

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(ButtonGrid::Type::MULTIPLE_SELECTION);

	stepTime = totalTime = 0.0f;
}

void SceneMapGenerator::Restart()
{
	if (state != State::FINISHED)
		return;

	state = State::STOP; // set to STOP so that Play() can be called properly

	map->ResetMap();

	buttonGrid->UnSelectAll();

	stepTime = totalTime = 0.0f;
}

// -------------------------------
// --- DEBUG DRAW ---
void SceneMapGenerator::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import Tileset"))
			{
			}
			if (ImGui::MenuItem("Export Tileset"))
			{
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Export Map"))
			{
			}

			ImGui::EndMenu();
		}

		ImGui::Text("|");
		if (ImGui::MenuItem("Tile Manager"))
		{
			App->scene->SetCurrentScene("TileManager");
		}
	}
	ImGui::EndMainMenuBar();
}

void SceneMapGenerator::DrawPanel()
{
	static const int panelY = menuBarHeight;
	static const int sectionSpacing = 5;
	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(panelX, panelY), ImGuiCond_Always);

	if (!isPanelOpen)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::Begin("###closedPanel", NULL, flags | ImGuiWindowFlags_NoTitleBar))
		{
			if (ImGui::Button("<", ImVec2(panelWidth, 0)))
			{
				PanelToggled(true);
				isPanelOpen = true;
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();

		return;
	}

	if (ImGui::Begin("Options", &isPanelOpen, flags))
	{
		// --- Buttons
		DrawSectionButtons();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);

		// --- Debug Options
		DrawSectionOptions();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);

		// --- World Size
		if (state == State::STOP)
		{
			DrawSectionResize();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);
		}

		// --- Cell Inspection
		if (state == State::STOP)
			DrawSectionCellPresets();
		else
			DrawSectionCellInspector();
	}
	ImGui::End();

	if (!isPanelOpen)
		PanelToggled(false);
}

void SceneMapGenerator::DrawSectionButtons()
{
	static const int buttonSize = 32;
	static const int paddingX = (ImGui::GetWindowContentRegionWidth() - 3 * (buttonSize + 12)) / 2;

	// Step
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + paddingX);
	if (ImGui::ImageButton((ImTextureID)stepIcon, ImVec2(buttonSize, buttonSize)))
	{
		Step();
	}
	ImGui::SameLine();

	// Play - Restart
	ImTextureID icon = (state == State::FINISHED) ? (ImTextureID)restartIcon : (ImTextureID)playIcon;
	if (ImGui::ImageButton(icon, ImVec2(buttonSize, buttonSize)))
	{
		Restart();
		Play();
	}
	ImGui::SameLine();

	// Stop
	if (ImGui::ImageButton((ImTextureID)stopIcon, ImVec2(buttonSize, buttonSize)))
	{
		Stop();
	}
}

void SceneMapGenerator::DrawSectionOptions()
{
	ImGui::Text("Step Time: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f ms", stepTime);
	
	ImGui::Text("Total Time:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f ms", totalTime);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
	ImGui::Checkbox("Draw Textures", &isDrawTextures);

	if (ImGui::Checkbox("Draw Spacing", &isDrawSpaced))
		UpdateButtonGrid();
}

void SceneMapGenerator::DrawSectionResize()
{
	static bool isAspectRatio = true;
	static int mapWidth = width;
	static int mapHeight = height;
	static int halfWidth = ImGui::GetWindowContentRegionWidth() / 2;
	static int thirdWidth = ImGui::GetWindowContentRegionWidth() / 3;

	// Size
	ImGui::SetNextItemWidth(thirdWidth);
	if (ImGui::DragInt("Width", &mapWidth, 1.0f, 1, 1000) && isAspectRatio)
		mapHeight = mapWidth * heightRatio;

	ImGui::SetNextItemWidth(thirdWidth);
	if (ImGui::DragInt("Height", &mapHeight, 1.0f, 1, 1000) && isAspectRatio)
		mapWidth = mapHeight * widthRatio;

	// Aspect Ratio
	ImGui::Checkbox("Keep aspect ratio", &isAspectRatio);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

	// Cancel Buttons
	if (ImGui::Button("Cancel", ImVec2(halfWidth, 0)))
	{
		mapWidth = width;
		mapHeight = height;
	}
	ImGui::SameLine(0.0f, 1.0f);

	// Apply Button
	if (ImGui::Button("Apply", ImVec2(halfWidth, 0)))
	{
		if (mapWidth == width && mapHeight == height)
			return;

		MapResized(mapWidth, mapHeight);
	}
}

void SceneMapGenerator::DrawSectionCellPresets()
{
	// Num Selected
	ImGui::Text("Selected Cells: %d", buttonGrid->GetSelected().size());
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

	// Reset Selected
	static int halfWidth = ImGui::GetWindowContentRegionWidth() / 2;
	if (ImGui::Button("Reset Sel.", ImVec2(halfWidth, 0)))
	{
		ClearCells(buttonGrid->GetSelected());
	}
	ImGui::SameLine(0.0f, 1.0f);

	// Reset All
	if (ImGui::Button("Reset All", ImVec2(halfWidth, 0)))
	{
		buttonGrid->UnSelectAll();
		map->ClearPresetCells();
		isMapPreset = false;
	}

	// Tile Picker
	if (buttonGrid->GetSelected().empty())
		return;

	List<unsigned int> tileArray;
	for (unsigned int i = 0; i < numTiles; ++i)
		tileArray.append(i);

	DrawCellInspector(tileArray);
}

void SceneMapGenerator::DrawSectionCellInspector()
{
	static const ImVec4 yellow = { 1, 1, 0, 1 };

	if (buttonGrid->GetSelected().empty())
		return;

	// Cell
	int index = buttonGrid->GetSelected().front();
	Cell* cell = map->GetCell(index);

	// Texture
	ImTextureID texture = 0;
	if (cell->isCollapsed && !cell->isInvalid)
		texture = (ImTextureID)map->GetTile(cell->tileID)->GetTexture();

	ImGui::Columns(3, "Cell Inspection", false);
	ImGui::SetColumnWidth(0, 60);
	ImGui::Image(texture, ImVec2(50, 50));
	ImGui::NextColumn();

	// Data
	ImGui::SetColumnWidth(0, 65);
	ImGui::Text("Index:");
	ImGui::Text("Position:");
	ImGui::Text("Collapsed:");
	ImGui::Text("TileID:");
	ImGui::Text("Mask:");
	ImGui::NextColumn();

	// Values
	ImGui::TextColored(yellow, "%d", index);
	ImGui::TextColored(yellow, "%d, %d", index % width, index / width);
	ImGui::TextColored(yellow, (cell->isCollapsed) ? "true" : "false");
	ImGui::TextColored(yellow, "%d", cell->tileID);
	ImGui::TextColored(yellow, MaskToString(cell->mask).c_str());
	ImGui::Columns(1);

	// Inspector
	if (state == State::FINISHED)
		return;

	List<unsigned int> setBits;
	for (unsigned int i = 0; i < cell->mask.size(); ++i)
	{
		if (cell->mask.getBit(i) == true)
			setBits.append(i);
	}

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	DrawCellInspector(setBits);
}

void SceneMapGenerator::DrawCellInspector(const List<unsigned int>& tileArray)
{
	// --- Tiles Selector
	static const int imageWidth = 30;
	static const int numColumns = 4;
	static const int menuBarHeight = 32;
	static const int itemSpacing = 10;

	// Height
	int numRows = tileArray.size() / numColumns;
	if (tileArray.size() % numColumns != 0)
		numRows++;

	int childHeight = numRows * (imageWidth + itemSpacing) + menuBarHeight;
	childHeight = MIN(childHeight, ImGui::GetContentRegionAvail().y);

	// Content
	if (ImGui::BeginChild("Mask Inspector", ImVec2(0, childHeight), true, ImGuiWindowFlags_MenuBar))
	{
		// Search Bar
		if (ImGui::BeginMenuBar())
		{
			ImGui::EndMenuBar();
		}

		// Tiles
		int count = 0;
		for (int i = 0; i < tileArray.size(); ++i)
		{
			unsigned int index = tileArray.at(i);
			ImTextureID texture = (ImTextureID)map->GetTile(index)->GetTexture();
			if (ImGui::ImageButton(texture, ImVec2(imageWidth, imageWidth)))
			{
				if (state == State::STOP)
				{
					PresetCells(buttonGrid->GetSelected(), index);
					isMapPreset = true;
				}
				else
				{
					map->SetCell(buttonGrid->GetSelected().front(), index);
					buttonGrid->UnSelectAll();
				}
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

// ------------------------------------------------
// --- EVENTS ---
void SceneMapGenerator::OnResize(EventWindowResize* e)
{
	panelX = e->width - panelWidth;
	panelHeight = e->height - menuBarHeight;

	bgButton->SetSize(panelX, e->height);

	blockButton->SetPosX(panelX);
	blockButton->SetSize(panelWidth, panelHeight);

	UpdateButtonGrid();

	camera->UpdateProjectionMatrix(0.0f, e->width * camera->GetZoom(), e->height * camera->GetZoom(), 0.0f);
}

void SceneMapGenerator::OnZoom(EventCameraZoom* e)
{
	float zoom = camera->GetZoom();
	zoom += (e->zoom > 0) ? -controller->GetZoomSpeed() : controller->GetZoomSpeed();
	
	controller->SetZoom(zoom);
}
