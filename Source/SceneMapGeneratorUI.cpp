#include "SceneMapGeneratorUI.h"
#include "SceneMapGenerator.h"
#include "SceneEvents.h"
#include "Event.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"

#include "Cell.h"
#include "Tile.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "SDL/include/SDL_scancode.h"

#include "mmgr/mmgr.h"

SceneMapGeneratorUI::SceneMapGeneratorUI(SceneMapGenerator* sceneMapGenerator, const DynArray<unsigned int>& tileTextures) 
	: sceneMap(sceneMapGenerator), tileTextures(tileTextures)
{
}

SceneMapGeneratorUI::~SceneMapGeneratorUI()
{
}

bool SceneMapGeneratorUI::Init()
{
	widthRatio = 1.0f;
	heightRatio = 1.0f;

	isDrawTextures = true;
	isDrawSpaced = true;

	isPanelOpen = true;
	panelX = App->window->GetWidth() - widthOpen;
	panelY = menuBarHeight;
	panelWidth = widthOpen;
	panelHeight = App->window->GetHeight() - panelY;

	// --- Events
	App->event->Subscribe(this, &SceneMapGeneratorUI::OnWindowResize);
	App->event->Subscribe(this, &SceneMapGeneratorUI::OnMapResize);
	App->event->Subscribe(this, &SceneMapGeneratorUI::OnPanelToggled);

    return true;
}

bool SceneMapGeneratorUI::Start()
{
	// --- Icons
	playIcon = App->resources->LoadTexture("Assets/Textures/Icons/play.png")->index;
	stepIcon = App->resources->LoadTexture("Assets/Textures/Icons/step.png")->index;
	stopIcon = App->resources->LoadTexture("Assets/Textures/Icons/stop.png")->index;
	restartIcon = App->resources->LoadTexture("Assets/Textures/Icons/restart.png")->index;

    return true;
}

bool SceneMapGeneratorUI::CleanUp()
{
    return true;
}

bool SceneMapGeneratorUI::Draw(int width, int height, Cell* cell)
{
	DrawMenuBar();
	DrawPanel(width, height, cell);

    return true;
}

// -----------------------------
// --- DRAW ---
void SceneMapGeneratorUI::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Tileset"))
			{
				App->event->Publish(new EventExportTileset());
			}
			if (ImGui::MenuItem("Save Map"))
			{
				App->event->Publish(new EventExportMap());
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Import Tile"))
			{
				App->event->Publish(new EventOpenImport());
			}
			if (ImGui::MenuItem("Import Tileset"))
			{
				App->event->Publish(new EventOpenImport());
			}
			if (ImGui::MenuItem("Import Map"))
			{
				App->event->Publish(new EventOpenImport());
			}

			ImGui::EndMenu();
		}

		ImGui::Text("|");
		if (ImGui::MenuItem("Tile Manager"))
		{
			App->event->Publish(new EventChangeScene("TileManager"));
		}
	}
	ImGui::EndMainMenuBar();
}

void SceneMapGeneratorUI::DrawPanel(int width, int height, Cell* cell)
{
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
				App->event->Publish(new EventPanelToggled(true));
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
		if (sceneMap->GetState() == SceneMapGenerator::State::STOP)
		{
			DrawSectionResize(width, height);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sectionSpacing);
		}

		// --- Cell Inspection
		if (sceneMap->GetState() == SceneMapGenerator::State::STOP)
			DrawSectionCellPresets();
		else
			DrawSectionCellInspector(width, cell);
	}
	ImGui::End();

	if (!isPanelOpen)
		App->event->Publish(new EventPanelToggled(false));
}

void SceneMapGeneratorUI::DrawSectionButtons()
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
	ImTextureID icon = (sceneMap->GetState() == SceneMapGenerator::State::FINISHED) ? (ImTextureID)restartIcon : (ImTextureID)playIcon;
	if (ImGui::ImageButton(icon, ImVec2(buttonSize, buttonSize)))
	{
		if (sceneMap->GetState() == SceneMapGenerator::State::FINISHED) // Restart
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

void SceneMapGeneratorUI::DrawSectionOptions()
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
	{
		App->event->Publish(new EventDrawSpaced(isDrawSpaced));
	}
}

void SceneMapGeneratorUI::DrawSectionResize(int width, int height)
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

void SceneMapGeneratorUI::DrawSectionCellPresets()
{
	// Num Selected
	ImGui::Text("Selected Cells: %d", sceneMap->GetNumSelected());
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

	// Reset Selected
	static int halfWidth = ImGui::GetWindowContentRegionWidth() / 2;
	if (ImGui::Button("Reset Sel.", ImVec2(halfWidth, 0)))
	{
		App->event->Publish(new EventResetCells(sceneMap->GetSelected()));
	}
	ImGui::SameLine(0.0f, 1.0f);

	// Reset All
	if (ImGui::Button("Reset All", ImVec2(halfWidth, 0)))
	{
		App->event->Publish(new EventResetAllCells());
	}

	// Tile Picker
	if (sceneMap->GetSelected().empty())
		return;

	DynArray<unsigned int> tileIDs;
	for (unsigned int i = 0; i < tileTextures.size(); ++i)
		tileIDs.push_back(i);

	DrawCellInspector(tileIDs);
}

void SceneMapGeneratorUI::DrawSectionCellInspector(int width, Cell* cell)
{
	static const ImVec4 yellow = { 1, 1, 0, 1 };

	if (sceneMap->GetSelected().empty())
		return;

	// Texture
	ImTextureID texture = 0;
	if (cell->isCollapsed && !cell->isInvalid)
		texture = (ImTextureID)tileTextures[cell->tileID];

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
	ImGui::TextColored(yellow, "%d, %d", cell->index % width, cell->index / width);
	ImGui::TextColored(yellow, (cell->isCollapsed) ? "true" : "false");
	ImGui::TextColored(yellow, "%d", cell->tileID);
	ImGui::TextColored(yellow, MaskToString(cell->mask).c_str());
	ImGui::Columns(1);

	// Inspector
	if (sceneMap->GetState() == SceneMapGenerator::State::FINISHED)
		return;

	DynArray<unsigned int> tileIDs;
	for (unsigned int i = 0; i < cell->mask.size(); ++i)
	{
		if (cell->mask.getBit(i) == true)
			tileIDs.push_back(i);
	}

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	DrawCellInspector(tileIDs);
}

void SceneMapGeneratorUI::DrawCellInspector(const DynArray<unsigned int>& tileIDs)
{
	// --- Tiles Selector
	static const int imageWidth = 30;
	static const int numColumns = 4;
	static const int menuBarHeight = 32;
	static const int itemSpacing = 10;

	// Height
	int numRows = tileIDs.size() / numColumns;
	if (tileIDs.size() % numColumns != 0)
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
		for (int i = 0; i < tileIDs.size(); ++i)
		{
			unsigned int index = tileIDs[i];
			ImTextureID texture = (ImTextureID)tileTextures[index];
			if (ImGui::ImageButton(texture, ImVec2(imageWidth, imageWidth)))
			{
				if (sceneMap->GetState() == SceneMapGenerator::State::STOP)
				{
					App->event->Publish(new EventPresetCells(sceneMap->GetSelected(), index));
				}
				else
				{
					App->event->Publish(new EventSetCell(sceneMap->GetSelected().front(), index));
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

// -----------------------------
// --- EVENTS ---
void SceneMapGeneratorUI::OnWindowResize(EventWindowResize* e)
{
	panelX = e->width - panelWidth;
	panelHeight = e->height - panelY;

	App->event->Publish(new EventPanelResize(panelX, panelWidth, panelHeight));
}

void SceneMapGeneratorUI::OnMapResize(EventMapResize* e)
{
	widthRatio = (float)e->width / (float)e->height;
	heightRatio = (float)e->height / (float)e->width;
}

void SceneMapGeneratorUI::OnPanelToggled(EventPanelToggled* e)
{
	panelWidth = (e->isOpen) ? widthOpen : widthClosed;
	panelX = App->window->GetWidth() - panelWidth;

	App->event->Publish(new EventPanelResize(panelX, panelWidth, panelHeight));
}

// -----------------------------
// --- UTILS ---
String SceneMapGeneratorUI::MaskToString(const BitArray& mask)
{
	String string = String(mask.size());

	for (unsigned int i = 0; i < mask.size(); ++i)
	{
		string += (mask.getBit(i)) ? '1' : '0';
	}

	return string;
}
