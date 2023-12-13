#include "SceneTileManagerUI.h"
#include "SceneTileManager.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"

#include "Tile.h"

#include "String.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

SceneTileManagerUI::SceneTileManagerUI(SceneTileManager* sceneTiles) : sceneTiles(sceneTiles)
{
}

SceneTileManagerUI::~SceneTileManagerUI()
{
}

bool SceneTileManagerUI::Init()
{
	currentTile = 0;
	currentDir = 0;
	isFilter = false;

	// --- Events
	App->event->Subscribe(this, &SceneTileManagerUI::OnImportTile);

	return true;
}

bool SceneTileManagerUI::Start()
{
	return true;
}

bool SceneTileManagerUI::CleanUp()
{
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
		if (ImGui::ImageButton(NULL, ImVec2(buttonSize, buttonSize))) // Save
		{
			// if (isAppend) //***
			App->event->Publish(new EventSaveTile(tileData[currentTile].tileID));
			// else
			// App->event->Publish(new EventUpdateTileset();
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(NULL, ImVec2(buttonSize, buttonSize))) // Save All
		{
			App->event->Publish(new EventSaveAllTiles());
			// App->event->Publish(new EventUpdateTileset(); //***
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(NULL, ImVec2(buttonSize, buttonSize))) // Import
		{
			App->event->Publish(new EventOpenImport());
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(NULL, ImVec2(buttonSize, buttonSize))) // Filter
		{
			isFilter = !isFilter;
		}
		ImGui::SameLine();
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
		if (!tileData.empty())
			tileTexture = (ImTextureID)tileData[currentTile].texture;
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
		BitArray mask = sceneTiles->GetTile(currentTile)->GetMasks()[currentDir];
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", MaskToString(mask).c_str());

		// Neighbours
		for (unsigned int i = 0; i < tileData.size(); ++i)
		{
			TileData tile = tileData[currentTile];
			TileData* neighbour = &tileData[i];

			if (NeighbourCombo(neighbour->name.c_str(), &neighbour->isSelected, 40, tile.texture, neighbour->texture, currentDir))
			{
				//neighbour->isSelected = true;
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
	TileData data;
	data.name = String("Tile %d", tileData.size());
	data.filepath = ""; //***
	data.tileID = e->tile->GetID();
	data.texture = e->tile->GetTexture();
	data.isSelected = false;
	data.isChanged = false;

	tileData.push_back(data);
}

// -----------------------------
// --- UTILS ---
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

bool SceneTileManagerUI::NeighbourCombo(const char* name, bool* selected, float texSize, unsigned int tex1, unsigned int tex2, unsigned int orientation)
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
		*selected = !*selected;
		ret = true;
	}

	ImU32 bgColor;
	if (ImGui::IsItemHovered())
		bgColor = *selected ? selectedHoveredColor : idleHoveredColor;
	else
		bgColor = *selected ? selectedColor : idleColor;

	ImU32 borderColor = *selected ? borderHoveredColor : idleColor;

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), bgColor, rounding);
	draw_list->AddRect(p, ImVec2(p.x + width, p.y + height), borderColor, rounding, ImDrawCornerFlags_All, 2.0f);


	// --- Textures
	float offset = padding + (texSize / 2.0f);
	if (orientation == 0) // top
	{
		ImVec2 pMin = ImVec2(p.x + offset, p.y + padding);
		ImVec2 pMax = ImVec2(pMin.x + texSize, pMin.y + texSize);

		draw_list->AddImage((ImTextureID)tex1, pMin, pMax);
		draw_list->AddImage((ImTextureID)tex2, ImVec2(pMin.x, pMax.y + spacing), ImVec2(pMax.x, pMax.y + spacing + texSize));
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

		draw_list->AddImage((ImTextureID)tex2, pMin, pMax);
		draw_list->AddImage((ImTextureID)tex1, ImVec2(pMin.x, pMax.y + spacing), ImVec2(pMax.x, pMax.y + + spacing + texSize));
	}

	return ret;
}

void SceneTileManagerUI::HierarchyNode()
{
}
