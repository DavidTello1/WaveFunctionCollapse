#include "SceneTiles.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"

#include "Tileset.h"
#include "Tile.h"

#include "String.h"
#include "Utils.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

SceneTiles::SceneTiles()
{
}

SceneTiles::~SceneTiles()
{
}

bool SceneTiles::Init()
{
	currentTile = 0;
	currentDir = 0;
	renameNode = -1;
	isFilter = false;
	isChanges = false;

	// --- Icons
	saveIcon = App->resources->LoadTexture("Assets/Textures/Icons/save.png")->index;
	saveAllIcon = App->resources->LoadTexture("Assets/Textures/Icons/saveAll.png")->index;
	importIcon = App->resources->LoadTexture("Assets/Textures/Icons/import.png")->index;
	filterIcon = App->resources->LoadTexture("Assets/Textures/Icons/filter.png")->index;

	return true;
}

bool SceneTiles::CleanUp()
{
	return true;
}

bool SceneTiles::DrawUI(const Tileset* tileset)
{
	DrawMenuBar();
	DrawToolbar();
	DrawTile(tileset->GetTile(currentTile)->GetTexture());
	DrawHierarchy();
	DrawMainPanel(tileset);

	return true;
}

// -----------------------
void SceneTiles::ImportTile(unsigned int tileID, const char* name, const char* texturePath)
{
	String tileName(name);
	if (ExistsName(name) || tileName == "")
		tileName = CreateName();

	TileData data;
	data.name = tileName;
	data.texturePath = texturePath;
	data.tileID = tileID;
	data.isChanged = false;

	tileData.push_back(data);
	isChanges = true;
}

// -----------------------
void SceneTiles::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import Tile"))
			{
				App->event->Publish(new EventImportTile());
			}
			if (ImGui::MenuItem("Import Tileset"))
			{
				App->event->Publish(new EventImportTileset());
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Export Tileset"))
			{
				App->event->Publish(new EventExportTileset());
			}

			ImGui::EndMenu();
		}

		ImGui::Text("|");
		String label = "Map Generator";
		if (isChanges)
			label += '*';
		if (ImGui::MenuItem(label.c_str()))
		{
			App->event->Publish(new EventChangeScene("MapGenerator"));
		}
	}
	ImGui::EndMainMenuBar();
}

void SceneTiles::DrawToolbar()
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
			if (isChanges)
			{
				App->event->Publish(new EventSaveTileset());

				for (unsigned int i = 0; i < tileData.size(); ++i)
					tileData[i].isChanged = false;
				isChanges = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)importIcon, ImVec2(buttonSize, buttonSize))) // Import
		{
			App->event->Publish(new EventImportAny());
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

void SceneTiles::DrawTile(unsigned int texture)
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
		if (texture != -1)
			tileTexture = (ImTextureID)texture;
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

void SceneTiles::DrawHierarchy()
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
			if (HierarchyNode(tileData[i].name.c_str(), currentTile == i, renameNode == i, tileData[i].isChanged))
				currentTile = i;
		}

		// Right Click
		if (ImGui::BeginPopup("TileOptions"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				renameNode = currentTile;
				isRenameFocus = true;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete"))
			{
				App->event->Publish(new EventRemoveTile(currentTile));
				tileData.erase(currentTile); //***
				isChanges = true;
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void SceneTiles::DrawMainPanel(const Tileset* tileset)
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

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", Utils::MaskToString(mask).c_str());

		// Neighbours
		static const int textureSize = 40;
		static const float itemSize = (textureSize + 8.0f) * 2.0f + 4.0f + 9.0f; // values from NeighbourCombo (padding & spacing)
		int columns =  (int)((float)mainPanelWidth / itemSize);
		for (unsigned int i = 0; i < tileset->GetSize(); ++i)
		{
			bool selected = mask.getBit(i);

			if (isFilter && !selected)
				continue;

			const Tile* neighbour = tileset->GetTile(i);

			String label = String("###%d", i);
			if (NeighbourCombo(label.c_str(), selected, textureSize, tile->GetTexture(), neighbour->GetTexture(), currentDir))
			{
				App->event->Publish(new EventUpdateMask(currentTile, currentDir, i, !selected));
				isChanges = true;
				tileData[currentTile].isChanged = true;
			}

			if (columns > 0 && (i + 1) % columns != 0)
				ImGui::SameLine();
		}
	}
	ImGui::End();
}

// -----------------------------
bool SceneTiles::TileButton(const char* name, bool selected, float width, float height)
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

bool SceneTiles::NeighbourCombo(const char* name, bool selected, float texSize, unsigned int tex1, unsigned int tex2, unsigned int orientation)
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

bool SceneTiles::HierarchyNode(const char* name, bool selected, bool rename, bool changes)
{
	bool ret = false;

	if (rename)
	{
		char buffer[128];
		sprintf_s(buffer, 128, "%s", name);

		if (ImGui::InputText("##RenameNode", buffer, 128, ImGuiInputTextFlags_AutoSelectAll))
		{
			tileData[currentTile].name = buffer;
		}

		if (isRenameFocus)
		{
			ImGui::SetKeyboardFocusHere(-1);
			isRenameFocus = false;
		}
		else if (!ImGui::IsItemActive())
			renameNode = -1;
	}
	else
	{
		static const ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAvailWidth;

		String label = name;
		if (changes)
			label += '*';

		if (ImGui::Selectable(label.c_str(), selected, flags))
			ret = true;

		// Right Click
		if (ImGui::OpenPopupOnItemClick("TileOptions"))
			ret = true;
	}

	return ret;
}

String SceneTiles::CreateName()
{
	int nameCount = 1;
	String name = String("Tile %d", nameCount);
	
	while (ExistsName(name.c_str()))
	{
		nameCount++;
		name = String("Tile %d", nameCount);
	}

	return name;
}

bool SceneTiles::ExistsName(const char* name)
{
	for (unsigned int i = 0; i < tileData.size(); ++i)
	{
		if (tileData[i].name == name)
			return true;
	}
	return false;
}
