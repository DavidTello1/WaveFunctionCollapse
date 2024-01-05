#include "SceneMap.h"
#include "Event.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"
#include "ModuleRenderer.h"

#include "MapGenerator.h"
#include "Cell.h"
#include "Tile.h"

#include "Camera.h"
#include "CameraController.h"
#include "UI_ButtonGroup.h"
#include "UI_Button.h"

#include "Utils.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "SDL/include/SDL_scancode.h"

#include "mmgr/mmgr.h"

SceneMap::SceneMap()
{
}

SceneMap::~SceneMap()
{
}

bool SceneMap::Init(const MapGenerator* map)
{
	state = State::STOP;

	// --- UI Variables
	stepTime = 0.0f;
	totalTime = 0.0f;
	widthRatio = 1.0f;
	heightRatio = 1.0f;
	spacing = defaultSpacing;
	isDrawSpaced = true;

	// --- Panel Data
	isPanelOpen = true;
	panelX = App->window->GetWidth() - widthOpen;
	panelY = menuBarHeight;
	panelWidth = widthOpen;
	panelHeight = App->window->GetHeight() - panelY;

	// --- Entities
	camera = new Camera(glm::vec3(0.0f), 0.0f, 1.0f);
	controller = new CameraController(camera, 0.3f);

	buttonGrid = new UI_ButtonGroup(0, 0, map->GetWidth(), map->GetHeight(), map->GetCellSize(), spacing, UI_ButtonGroup::Type::MULTIPLE_SELECTION);
	UpdateButtonGrid();

	Color transparent = { 1,0,0,0 };
	bgButton = new UI_Button(0, panelY, panelX, App->window->GetHeight(), transparent, transparent, transparent, true);

	// --- Icons
	playIcon = App->resources->LoadTexture("Assets/Textures/Icons/play.png")->index;
	stepIcon = App->resources->LoadTexture("Assets/Textures/Icons/step.png")->index;
	stopIcon = App->resources->LoadTexture("Assets/Textures/Icons/stop.png")->index;
	restartIcon = App->resources->LoadTexture("Assets/Textures/Icons/restart.png")->index;

    return true;
}

bool SceneMap::Update(float dt)
{
	controller->Update(dt);

	buttonGrid->Update(dt);
	bgButton->Update(dt);

	if (!buttonGrid->IsHovered() && bgButton->IsClicked() &&
		App->input->GetKey(SDL_SCANCODE_RCTRL) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT &&
		App->input->GetKey(SDL_SCANCODE_RALT) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
	{
		buttonGrid->UnSelectAll();
	}

	// --- Shortcuts
	Shortcuts();

	return true;
}

bool SceneMap::CleanUp()
{
	delete camera;
	delete controller;
	delete buttonGrid;
	delete bgButton;

    return true;
}

bool SceneMap::Draw(const MapGenerator* map)
{
	DrawMap(map);
	buttonGrid->Draw();

	return true;
}

bool SceneMap::DrawUI(const MapGenerator* map)
{
	DrawPanel(map);

    return true;
}

// -----------------------------
void SceneMap::UnselectAllCells()
{
	buttonGrid->UnSelectAll();
}

void SceneMap::OnWindowResize(int width, int height)
{
	// Update entities
	UpdateButtonGrid();
	bgButton->SetSize(width - panelWidth, height - panelY);

	// Udpate panel position
	panelX = width - panelWidth;
	panelHeight = height - panelY;
}

void SceneMap::OnZoom(float newZoom)
{
	float zoom = camera->GetZoom();
	zoom += (newZoom > 0) ? -controller->GetZoomSpeed() : controller->GetZoomSpeed();

	controller->SetZoom(zoom);
}

void SceneMap::OnMapResize(float width, float height)
{
	buttonGrid->Resize(width, height);
	UpdateButtonGrid();

	widthRatio = width / height;
	heightRatio = height / width;
}

void SceneMap::OnPlay()
{
	if (state != State::STOP && state != State::PAUSE)
		return;
	state = State::PLAY;

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(UI_ButtonGroup::Type::SINGLE_SELECTION);
}

void SceneMap::OnStep()
{
	if (state != State::STOP && state != State::PAUSE)
		return;
	state = State::PAUSE;

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(UI_ButtonGroup::Type::SINGLE_SELECTION);
}

void SceneMap::OnStop()
{
	if (state == State::STOP)
		return;
	state = State::STOP;

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(UI_ButtonGroup::Type::MULTIPLE_SELECTION);
}

// -----------------------------
void SceneMap::Shortcuts()
{
	// --- Shortcuts
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) // Step (spacebar)
	{
		App->event->Publish(new EventStep());
	}
	else if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) // Play (F1)
	{
		if (state == State::FINISHED) // Restart
			App->event->Publish(new EventStop());

		App->event->Publish(new EventPlay());
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) // Stop (F2)
	{
		App->event->Publish(new EventStop());
	}
}

void SceneMap::DrawSpaced()
{
	spacing = (isDrawSpaced) ? defaultSpacing : 0;

	buttonGrid->SetSpacing(spacing);
	UpdateButtonGrid();
}

void SceneMap::PanelToggled(bool isOpen)
{
	// Update Panel
	panelWidth = (isOpen) ? widthOpen : widthClosed;
	panelX = App->window->GetWidth() - panelWidth;

	// Update bgButton
	bgButton->SetWidth(panelX);
}

void SceneMap::UpdateButtonGrid()
{
	int width = buttonGrid->GetNumColumns();
	int height = buttonGrid->GetNumRows();
	int buttonSize = buttonGrid->GetButtonSize();

	int offsetX = ((int)App->window->GetWidth() - ((width + spacing) * buttonSize)) / 2;
	int offsetY = ((int)App->window->GetHeight() - ((height + spacing) * buttonSize)) / 2;

	buttonGrid->SetPos(offsetX, offsetY);
}

void SceneMap::DrawMap(const MapGenerator* map)
{
	static const Color black = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const Color gray = { 0.5f, 0.5f, 0.5f, 1.0f };
	static const Color red = { 1.0f, 0.0f, 0.0f, 1.0f };
	static const Color green = { 0.0f, 1.0f, 0.0f, 1.0f };

	const int width = map->GetWidth();
	const int height = map->GetHeight();
	const int cellSize = map->GetCellSize();

	// Offset
	int offsetX = ((int)App->window->GetWidth() - ((width + spacing) * cellSize)) / 2;
	int offsetY = ((int)App->window->GetHeight() - ((height + spacing) * cellSize)) / 2;

	// --- Draw Map ---
	int numCells = width * height;
	for (unsigned int i = 0; i < numCells; ++i)
	{
		// Cell
		Cell* cell = map->GetCell(i);
		int x = cell->index % width;
		int y = cell->index / width;

		// Position & Size
		glm::vec2 position = { offsetX + x * (cellSize + spacing), offsetY + y * (cellSize + spacing) };
		glm::vec2 size = { cellSize, cellSize };

		// Draw Texture
		if (!cell->isInvalid && (cell->isCollapsed || cell->isPreset))
		{
			Tile* tile = map->GetTileByID(cell->tileID);
			App->renderer->DrawQuad(position, size, tile->GetTexture());
			continue;
		}

		// Draw Color
		Color color = (cell->isInvalid) ? red : gray;
		App->renderer->DrawQuad(position, size, glm::vec4(color.r, color.g, color.b, color.a));
	}
}

// -----------------------------
// --- UI DRAW ---
void SceneMap::DrawPanel(const MapGenerator* map)
{
	static const int sectionSpacing = 5;
	static const ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_AlwaysAutoResize | 
		ImGuiWindowFlags_NoCollapse;

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
			DrawSectionResize(map->GetWidth(), map->GetHeight());

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);
		}

		// --- Cell Inspection
		if (state == State::STOP)
			DrawSectionCellPresets(map);
		else
			DrawSectionCellInspector(map);
	}
	ImGui::End();

	if (!isPanelOpen)
		PanelToggled(false);
}

void SceneMap::DrawSectionButtons()
{
	static const int buttonSize = 32;
	static const int paddingX = (ImGui::GetWindowContentRegionWidth() - 3 * (buttonSize + 12)) / 2;

	// Step
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + paddingX);
	if (ImGui::ImageButton((ImTextureID)stepIcon, ImVec2(buttonSize, buttonSize)))
	{
		App->event->Publish(new EventStep());
	}
	ImGui::SameLine();

	// Play - Restart
	ImTextureID icon = (state == State::FINISHED) ? (ImTextureID)restartIcon : (ImTextureID)playIcon;
	if (ImGui::ImageButton(icon, ImVec2(buttonSize, buttonSize)))
	{
		if (state == State::FINISHED) // Restart
			App->event->Publish(new EventStop());

		App->event->Publish(new EventPlay());
	}
	ImGui::SameLine();

	// Stop
	if (ImGui::ImageButton((ImTextureID)stopIcon, ImVec2(buttonSize, buttonSize)))
	{
		App->event->Publish(new EventStop());
	}
}

void SceneMap::DrawSectionOptions()
{
	ImGui::Text("Step Time: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f ms", stepTime);

	ImGui::Text("Total Time:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f ms", totalTime);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
	if (ImGui::Checkbox("Draw Spacing", &isDrawSpaced))
	{
		DrawSpaced();
	}
}

void SceneMap::DrawSectionResize(int width, int height)
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

		App->event->Publish(new EventMapResize(mapWidth, mapHeight));
	}
}

void SceneMap::DrawSectionCellPresets(const MapGenerator* map)
{
	// Num Selected
	ImGui::Text("Selected Cells: %d", buttonGrid->GetSelected().size());
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

	// Reset Selected
	static int halfWidth = ImGui::GetWindowContentRegionWidth() / 2;
	if (ImGui::Button("Reset Sel.", ImVec2(halfWidth, 0)))
	{
		App->event->Publish(new EventResetCells(buttonGrid->GetSelected()));
	}
	ImGui::SameLine(0.0f, 1.0f);

	// Reset All
	if (ImGui::Button("Reset All", ImVec2(halfWidth, 0)))
	{
		App->event->Publish(new EventResetAllCells());
	}

	// Tile Picker
	if (buttonGrid->GetSelected().size() == 0)
		return;

	DrawCellInspector(map->GetAllTiles());
}

void SceneMap::DrawSectionCellInspector(const MapGenerator* map)
{
	static const int maxTextSize = 105;
	static const ImVec4 yellow = { 1, 1, 0, 1 };

	if (buttonGrid->GetSelected().size() == 0)
		return;

	// Cell
	Cell* cell = map->GetCell(buttonGrid->GetSelected().front());
	
	// Texture
	ImTextureID texture = 0;
	if (cell->isCollapsed && !cell->isInvalid)
	{
		Tile* tile = map->GetTileByID(cell->tileID);
		if (tile != nullptr)
			texture = (ImTextureID)tile->GetTexture();
	}

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
	ImGui::TextColored(yellow, "%d", cell->index);
	ImGui::TextColored(yellow, "%d, %d", cell->index % map->GetWidth(), cell->index / map->GetWidth());
	ImGui::TextColored(yellow, (cell->isCollapsed) ? "true" : "false");
	DrawText(String("%d", cell->tileID).c_str(), maxTextSize);
	DrawText(Utils::MaskToString(cell->mask).c_str(), maxTextSize);
	ImGui::Columns(1);

	// Inspector
	if (cell->isCollapsed || state == State::FINISHED)
		return;

	DynArray<Tile*> tiles;
	for (unsigned int i = 0; i < cell->mask.size(); ++i)
	{
		if (cell->mask.getBit(i) == true)
			tiles.push_back(map->GetTile(i));
	}

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	DrawCellInspector(tiles);
}

void SceneMap::DrawCellInspector(const DynArray<Tile*>& tiles)
{
	// --- Tiles Selector
	static const int imageWidth = 30;
	static const int numColumns = 4;
	static const int menuBarHeight = 32;
	static const int itemSpacing = 10;

	// Height
	int numRows = tiles.size() / numColumns;
	if (tiles.size() % numColumns != 0)
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
		for (int i = 0; i < tiles.size(); ++i)
		{
			Tile* tile = tiles[i];
			if (ImGui::ImageButton((ImTextureID)tile->GetTexture(), ImVec2(imageWidth, imageWidth)))
			{
				if (state == State::STOP)
				{
					App->event->Publish(new EventPresetCells(buttonGrid->GetSelected(), tile->GetID()));
				}
				else
				{
					App->event->Publish(new EventSetCell(buttonGrid->GetSelected().front(), tile->GetID()));
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

void SceneMap::DrawText(const char* label, const int maxSize)
{
	static const int maxChars = 8;
	static const ImVec4 yellow = { 1, 1, 0, 1 };

	String text = label;

	float size = ImGui::CalcTextSize(label).x;
	if (size > maxSize)
	{
		text = text.substr(0, maxChars) + "...";
	}

	ImGui::TextColored(yellow, text.c_str());

	if (size > maxSize && ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text(label);
		ImGui::EndTooltip();
	}
}
