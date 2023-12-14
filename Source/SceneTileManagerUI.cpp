#include "SceneTileManagerUI.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"
#include "ModuleResources.h" //***

#include "Tile.h"

#include "String.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

SceneTileManagerUI::SceneTileManagerUI()
{
}

SceneTileManagerUI::~SceneTileManagerUI()
{
}

bool SceneTileManagerUI::Init()
{
	tileset = new Tileset();

	currentTile = 0;
	currentDir = 0;
	isFilter = false;

	// --- Events
	App->event->Subscribe(this, &SceneTileManagerUI::OnImportTile);

	return true;
}

bool SceneTileManagerUI::Start()
{
	// --- Icons
	saveIcon = App->resources->LoadTexture("Assets/Textures/Icons/save.png")->index;
	saveAllIcon = App->resources->LoadTexture("Assets/Textures/Icons/saveAll.png")->index;
	importIcon = App->resources->LoadTexture("Assets/Textures/Icons/import.png")->index;
	filterIcon = App->resources->LoadTexture("Assets/Textures/Icons/filter.png")->index;

	// -----------------------------------------
	Tile* empty = new Tile(0, App->resources->LoadTexture("Assets/Textures/empty.png")->index, "1001111", "1010111", "1101011", "1110011");
	Tile* topLeft = new Tile(1, App->resources->LoadTexture("Assets/Textures/topLeft.png")->index, "1000000", "1000000", "0010110", "0001101");
	Tile* topRight = new Tile(2, App->resources->LoadTexture("Assets/Textures/topRight.png")->index, "1000000", "0101010", "1000000", "0001101");
	Tile* bottomLeft = new Tile(3, App->resources->LoadTexture("Assets/Textures/bottomLeft.png")->index, "0110001", "1000000", "0010110", "1000000");
	Tile* bottomRight = new Tile(4, App->resources->LoadTexture("Assets/Textures/bottomRight.png")->index, "0110001", "0101010", "1000000", "1000000");
	Tile* horizontal = new Tile(5, App->resources->LoadTexture("Assets/Textures/horizontal.png")->index, "1000000", "1101010", "1010110", "1000000");
	Tile* vertical = new Tile(6, App->resources->LoadTexture("Assets/Textures/vertical.png")->index, "1110001", "1000000", "1000000", "1001101");

	tileset->AddTile(empty);
	tileset->AddTile(topLeft);
	tileset->AddTile(topRight);
	tileset->AddTile(bottomLeft);
	tileset->AddTile(bottomRight);
	tileset->AddTile(horizontal);
	tileset->AddTile(vertical);

	CreateTileData(empty);
	CreateTileData(topLeft);
	CreateTileData(topRight);
	CreateTileData(bottomLeft);
	CreateTileData(bottomRight);
	CreateTileData(horizontal);
	CreateTileData(vertical);
	//----------------------------------------

	return true;
}

bool SceneTileManagerUI::CleanUp()
{
	delete tileset;

	return true;
}

bool SceneTileManagerUI::Draw()
{
	DrawMenuBar();
	DrawToolbar();
	DrawTile();
	DrawHierarchy();
	DrawMainPanel();

	return true;
}

void SceneTileManagerUI::DrawMenuBar()
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
		if (ImGui::MenuItem("Map Generator"))
		{
			App->event->Publish(new EventChangeScene("MapGenerator"));
		}
	}
	ImGui::EndMainMenuBar();
}

void SceneTileManagerUI::DrawToolbar() //*** ICONS
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
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5, 0.5, 0.5, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5, 0.5, 0.5, 0.7f));

		if (ImGui::ImageButton((ImTextureID)saveIcon, ImVec2(buttonSize, buttonSize))) // Save
		{
			// if (isAppend) //***
			App->event->Publish(new EventSaveTile(tileset->GetTile(currentTile)));
			// else
			// App->event->Publish(new EventUpdateTileset();
		}
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)saveAllIcon, ImVec2(buttonSize, buttonSize))) // Save All
		{
			App->event->Publish(new EventSaveAllTiles());
			// App->event->Publish(new EventUpdateTileset(); //***
		}
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)importIcon, ImVec2(buttonSize, buttonSize))) // Import
		{
			App->event->Publish(new EventOpenImport());
		}
		ImGui::SameLine();

		bool filter = isFilter;
		if (filter)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.5, 0.5, 0.4f));

		if (ImGui::ImageButton((ImTextureID)filterIcon, ImVec2(buttonSize, buttonSize))) // Filter
		{
			isFilter = !isFilter;
		}

		if (filter)
			ImGui::PopStyleColor();

		ImGui::PopStyleColor(3);
	}
	ImGui::End();
}

void SceneTileManagerUI::DrawTile()
{
	static const int buttonSize = 40;
	static const int offset = 22; //(hierarchyWidth - (buttonSize * 3) / 2 - 8->padding

	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

	static const int tileX = 0;
	static const int tileY = menubarHeight + toolbarHeight;

	ImGui::SetNextWindowPos(ImVec2(tileX, tileY), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(hierarchyWidth, tileHeight), ImGuiCond_Always);

	if (ImGui::Begin("Tile", NULL, flags))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1, 0, 0, 1));

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
		ImGui::Dummy(ImVec2(buttonSize, buttonSize));
		ImGui::SameLine();
		if (TileButton("###Top", currentDir == 0, buttonSize, buttonSize))
			currentDir = 0;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
		if (TileButton("###Left", currentDir == 1, buttonSize, buttonSize))
			currentDir = 1;

		ImGui::SameLine();
		ImTextureID tileTexture = 0;
		if (tileset->GetSize() > 0)
			tileTexture = (ImTextureID)tileset->GetTile(currentTile)->GetTexture();
		ImGui::Image(tileTexture, ImVec2(buttonSize, buttonSize));
		ImGui::SameLine();

		if (TileButton("###Right", currentDir == 2, buttonSize, buttonSize))
			currentDir = 2;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
		ImGui::Dummy(ImVec2(buttonSize, buttonSize));
		ImGui::SameLine();
		if (TileButton("###Bottom", currentDir == 3, buttonSize, buttonSize))
			currentDir = 3;

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
	ImGui::End();
}

void SceneTileManagerUI::DrawHierarchy()
{
	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

	static const int hierarchyX = 0;
	static const int hierarchyY = menubarHeight + toolbarHeight + tileHeight;
	int hierarchyHeight = App->window->GetHeight() - hierarchyY;

	ImGui::SetNextWindowPos(ImVec2(hierarchyX, hierarchyY), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(hierarchyWidth, hierarchyHeight), ImGuiCond_Always);

	if (ImGui::Begin("Hierarchy", NULL, flags))
	{
		for (unsigned int i = 0; i < tileData.size(); ++i)
		{
			if (ImGui::Selectable(tileData[i].name.c_str(), currentTile == i, ImGuiSelectableFlags_SpanAvailWidth))
				currentTile = i;
		}
	}
	ImGui::End();
}

void SceneTileManagerUI::DrawMainPanel()
{
	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

	static const int mainPanelX = hierarchyWidth;
	static const int mainPanelY = menubarHeight + toolbarHeight;
	int mainPanelWidth = App->window->GetWidth() - mainPanelX;
	int mainPanelHeight = App->window->GetHeight() - mainPanelY;

	ImGui::SetNextWindowPos(ImVec2(mainPanelX, mainPanelY), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mainPanelWidth, mainPanelHeight), ImGuiCond_Always);

	if (ImGui::Begin("Neighbours", NULL, flags))
	{
		// Tile Mask

		const Tile* tile = tileset->GetTile(currentTile);
		const BitArray mask = tile->GetMasks()[currentDir];

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", MaskToString(mask).c_str());

		// Neighbours
		for (unsigned int i = 0; i < tileset->GetSize(); ++i)
		{
			bool selected = mask.getBit(i);

			if (isFilter && !selected)
				continue;

			const Tile* neighbour = tileset->GetTile(i);

			String label = String("###%d", i);
			if (NeighbourCombo(label.c_str(), selected, 40, tile->GetTexture(), neighbour->GetTexture(), currentDir))
			{
				tileset->UpdateMask(currentTile, currentDir, i, !selected);
			}
			ImGui::SameLine();
		}
	}
	ImGui::End();
}

// -----------------------------
// --- EVENTS ---
void SceneTileManagerUI::OnImportTile(EventImportTile* e)
{
	tileset->AddTile(e->tile);

	CreateTileData(e->tile);
}

// -----------------------------
// --- UTILS ---
void SceneTileManagerUI::CreateTileData(Tile* tile)
{
	TileData data;
	data.name = String("Tile %d", tileData.size());
	data.filepath = ""; //***
	data.tileID = tile->GetID();
	data.isSelected = false;
	data.isChanged = false;

	tileData.push_back(data);
}

String SceneTileManagerUI::MaskToString(const BitArray& mask)
{
	String string = String(mask.size());

	for (unsigned int i = 0; i < mask.size(); ++i)
	{
		string += (mask.getBit(i)) ? '1' : '0';
	}

	return string;
}

bool SceneTileManagerUI::TileButton(const char* name, bool selected, float width, float height)
{
	static const ImU32 idleColor			= IM_COL32(110, 110, 110, 255);
	static const ImU32 idleHoveredColor		= IM_COL32(140, 140, 140, 255);
	static const ImU32 selectedColor		= IM_COL32(50, 30, 200, 255);
	static const ImU32 selectedHoveredColor	= IM_COL32(50, 30, 255, 255);

	bool ret = false;

	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	if (ImGui::InvisibleButton(name, ImVec2(width, height)))
	{
		selected = !selected;
		ret = true;
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = selected ? selectedHoveredColor : idleHoveredColor;
	else
		col_bg = selected ? selectedColor : idleColor;

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg);

	return ret;
}

bool SceneTileManagerUI::NeighbourCombo(const char* name, bool selected, float texSize, unsigned int tex1, unsigned int tex2, unsigned int orientation)
{
	static const int rounding = 3.0f;
	static const int padding = 8.0f;
	static const int spacing = 4.0f;

	ImVec4 bg_color = ImVec4(0.4f, 0.7f, 1.0f, 0.0f);
	ImVec4 border_color = ImVec4(0.0f, 0.4f, 0.8f, 0.0f);

	static const ImU32 borderHoveredColor	= IM_COL32(0, 102, 204, 255);
	static const ImU32 idleColor			= IM_COL32(110, 110, 110, 0);
	static const ImU32 idleHoveredColor		= IM_COL32(110, 110, 110, 255);
	static const ImU32 selectedColor		= IM_COL32(102, 178, 225, 0);
	static const ImU32 selectedHoveredColor = IM_COL32(110, 110, 110, 255);

	bool ret = false;

	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float width = (texSize + padding) * 2.0f + spacing;
	float height = width;
	if (ImGui::InvisibleButton(name, ImVec2(width, height)))
	{
		ret = true;
	}

	ImU32 bgColor;
	if (ImGui::IsItemHovered())
		bgColor = selected ? selectedHoveredColor : idleHoveredColor;
	else
		bgColor = selected ? selectedColor : idleColor;

	ImU32 borderColor = selected ? borderHoveredColor : idleColor;

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), bgColor, rounding);
	draw_list->AddRect(p, ImVec2(p.x + width, p.y + height), borderColor, rounding, ImDrawCornerFlags_All, 2.0f);


	// --- Textures
	float offset = padding + (texSize / 2.0f);
	if (orientation == 0) // top
	{
		ImVec2 pMin = ImVec2(p.x + offset, p.y + padding);
		ImVec2 pMax = ImVec2(pMin.x + texSize, pMin.y + texSize);

		draw_list->AddImage((ImTextureID)tex2, pMin, pMax);
		draw_list->AddImage((ImTextureID)tex1, ImVec2(pMin.x, pMax.y + spacing), ImVec2(pMax.x, pMax.y + spacing + texSize));
	}
	else if (orientation == 1) // left
	{
		ImVec2 pMin = ImVec2(p.x + padding, p.y + offset);
		ImVec2 pMax = ImVec2(pMin.x + texSize, pMin.y + texSize);

		draw_list->AddImage((ImTextureID)tex2, pMin, pMax);
		draw_list->AddImage((ImTextureID)tex1, ImVec2(pMax.x + spacing, pMin.y), ImVec2(pMax.x + spacing + texSize, pMax.y));
	}
	else if (orientation == 2) // right
	{
		ImVec2 pMin = ImVec2(p.x + padding, p.y + offset);
		ImVec2 pMax = ImVec2(pMin.x + texSize, pMin.y + texSize);

		draw_list->AddImage((ImTextureID)tex1, pMin, pMax);
		draw_list->AddImage((ImTextureID)tex2, ImVec2(pMax.x + spacing, pMin.y), ImVec2(pMax.x + spacing + texSize, pMax.y));
	}
	else if (orientation == 3) // bottom
	{
		ImVec2 pMin = ImVec2(p.x + offset, p.y + padding);
		ImVec2 pMax = ImVec2(pMin.x + texSize, pMin.y + texSize);

		draw_list->AddImage((ImTextureID)tex1, pMin, pMax);
		draw_list->AddImage((ImTextureID)tex2, ImVec2(pMin.x, pMax.y + spacing), ImVec2(pMax.x, pMax.y + + spacing + texSize));
	}

	return ret;
}

void SceneTileManagerUI::HierarchyNode()
{
}
