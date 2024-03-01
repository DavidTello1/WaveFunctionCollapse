#include "SceneTiles.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleResources.h"

#include "Tileset.h"
#include "Tile.h"

#include "String.h"
#include "Utils.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "SDL/include/SDL_scancode.h"

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
	renameNode = -1;
	isFilter = false;
	isChanges = false;
	isTileData = true;
	isRenameFocus = false;

	// --- Icons
	saveIcon = App->resources->LoadTexture("Assets/Textures/Icons/save.png")->index;
	saveAllIcon = App->resources->LoadTexture("Assets/Textures/Icons/saveAll.png")->index;
	importIcon = App->resources->LoadTexture("Assets/Textures/Icons/import.png")->index;
	filterIcon = App->resources->LoadTexture("Assets/Textures/Icons/filter.png")->index;
	comboIcon = App->resources->LoadTexture("Assets/Textures/Icons/combo.png")->index;

	// ---
	comboData.dir = -1;
	comboData.firstIndex = -1;
	comboData.secondIndex = -1;

	return true;
}

bool SceneTiles::Update(float dt)
{
	Shortcuts();

	return true;
}

bool SceneTiles::CleanUp()
{
	return true;
}

bool SceneTiles::DrawUI(const Tileset* tileset)
{
	DrawToolbar();
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
	isTileData = true;
	currentTile = tileData.size() - 1;
}

void SceneTiles::ClearTileData()
{
	tileData.clear();
}

// -----------------------
void SceneTiles::DrawToolbar()
{
	static const ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoNav;

	static const int toolbarX = 0;
	static const int toolbarY = menubarHeight;
	static const int toolbarHeight = 46;

	static const int buttonSize = 24;

	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth(), toolbarHeight), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(toolbarX, toolbarY), ImGuiCond_Once);

	if (ImGui::Begin("Toolbar", NULL, flags))
	{

		if (ToolbarIcon(saveIcon, buttonSize, "Save Tileset")) // Save
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

		if (ToolbarIcon(importIcon, buttonSize, "Import File")) // Import
		{
			App->event->Publish(new EventImport());
		}
		ImGui::SameLine();

		if (ToolbarIcon(filterIcon, buttonSize, "Filter Selected", isFilter)) // Filter
		{
			isFilter = !isFilter;
		}
		ImGui::SameLine();

		if (ToolbarIcon(comboIcon, buttonSize, "Show Combinations", !isTileData)) // Combinations
		{
			isTileData = !isTileData;
		}
	}
	ImGui::End();
}

void SceneTiles::DrawHierarchy()
{
	static const ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoNav;

	static const int hierarchyX = 0;
	static const int hierarchyY = menubarHeight + toolbarHeight;
	int hierarchyHeight = App->window->GetHeight() - hierarchyY;

	ImGui::SetNextWindowPos(ImVec2(hierarchyX, hierarchyY), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(hierarchyWidth, hierarchyHeight), ImGuiCond_Always);

	if (ImGui::Begin("Tileset", NULL, flags))
	{
		for (unsigned int i = 0; i < tileData.size(); ++i)
		{
			if (HierarchyNode(tileData[i].name.c_str(), currentTile == i, renameNode == i, tileData[i].isChanged))
			{
				currentTile = i;
				isTileData = true;
			}
		}

		// Right Click
		if (ImGui::BeginPopup("TileOptions", ImGuiWindowFlags_NoMove))
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
				tileData.erase(currentTile);

				currentTile = 0;
				isChanges = true;
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void SceneTiles::DrawMainPanel(const Tileset* tileset)
{
	static const ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav;

	static const int mainPanelX = hierarchyWidth;
	static const int mainPanelY = menubarHeight + toolbarHeight;
	int mainPanelWidth = App->window->GetWidth() - mainPanelX;
	int mainPanelHeight = App->window->GetHeight() - mainPanelY;
	ImGui::SetNextWindowPos(ImVec2(mainPanelX, mainPanelY), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mainPanelWidth, mainPanelHeight), ImGuiCond_Always);

	ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.35f, 0.35f, 0.7f));

	String title = (isTileData) ? "Tile Data" : "Combinations";
	if (ImGui::Begin(title.c_str(), NULL, flags))
	{
		if (isTileData)
		{
			DrawTileData(tileset, mainPanelWidth);
		}
		else
		{
			DrawCombinations(tileset, mainPanelWidth);
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void SceneTiles::DrawTileData(const Tileset* tileset, const float panelWidth)
{
	static const int size = 60;
	static const int neighbourSize = 40;
	static const ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

	static bool topOpen = true;
	static bool leftOpen = true;
	static bool rightOpen = true;
	static bool bottomOpen = true;

	if (tileData.empty())
		return;

	const Tile* tile = tileset->GetTile(currentTile);
	TileData data = tileData[currentTile];

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
	ImGui::Columns(3, "Columns", false);
	ImGui::SetColumnWidth(0, size + 8);
	ImGui::Image((ImTextureID)tile->GetTexture(), ImVec2(size, size));

	ImGui::NextColumn();
	ImGui::SetColumnWidth(1, 64);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	ImGui::Text("Name:");
	ImGui::Text("Tile ID:");
	ImGui::Text("Texture: ");

	ImGui::NextColumn();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	ImGui::TextColored(yellow, data.name.c_str());
	ImGui::TextColored(yellow, "%d", data.tileID);
	ImGui::TextColored(yellow, data.texturePath.c_str());
	ImGui::EndColumns();

	bool isWalkable = tile->IsWalkable();
	if (ImGui::Checkbox("Walkable", &isWalkable))
		App->event->Publish(new EventSetTileWalkable(currentTile, isWalkable));

	bool isSymmetrical = tile->IsSymmetrical();
	if (ImGui::Checkbox("Symmetrical", &isSymmetrical))
		App->event->Publish(new EventSetTileSymmetry(currentTile, isSymmetrical));

	int cost = tile->GetCost();
	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputInt("Cost", &cost, 1, 1))
		App->event->Publish(new EventSetTileCost(currentTile, cost));


	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);
	DrawMask("Top Mask:    ", &topOpen,	   0, tileset, neighbourSize, panelWidth);
	DrawMask("Left Mask:   ", &leftOpen,   1, tileset, neighbourSize, panelWidth);
	DrawMask("Right Mask:  ", &rightOpen,  2, tileset, neighbourSize, panelWidth);
	DrawMask("Bottom Mask: ", &bottomOpen, 3, tileset, neighbourSize, panelWidth);
}

void SceneTiles::DrawMask(const char* name, bool* selected, const int dir, const Tileset* tileset, const int size, const float panelWidth)
{
	static const ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	static const int comboSize = 32;
	static const int itemSize = 64 + 2 + 25; // values from NeighbourCombo (padding & spacing)
	int columns = (int)(panelWidth / itemSize);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));

	const Tile* tile = tileset->GetTile(currentTile);
	const BitArray mask = tile->GetMasks()[dir];

	String arrowLbl = String("%s%d", name, tile->GetID());
	if (ImGui::ArrowButton(name, (*selected) ? ImGuiDir_Down : ImGuiDir_Right))
		*selected = !*selected;

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::Text(name);
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextColored(yellow, "%s", Utils::MaskToString(mask).c_str());

	if (!*selected)
		return;

	int count = 0;
	int lastBit = mask.lastSetBit();
	for (int i = 0; i < mask.size(); ++i)
	{
		bool isSet = mask.getBit(i);

		if (isFilter && !isSet)
			continue;

		const Tile* neighbour = tileset->GetTile(i);
		String label = String("%d%d%d", dir, tile->GetID(), neighbour->GetID());

		if (NeighbourCombo(label.c_str(), isSet, comboSize, tile->GetTexture(), neighbour->GetTexture(), dir))
		{
			// Update Tile Mask
			int neighbourDir = NUM_NEIGHBOURS - dir - 1;
			App->event->Publish(new EventUpdateMask(currentTile, dir, i, !isSet));

			isChanges = true;
			tileData[currentTile].isChanged = true;

			// Update Neighbour Mask
			if (tile->IsSymmetrical())
			{
				App->event->Publish(new EventUpdateMask(i, neighbourDir, currentTile, !isSet));
				tileData[i].isChanged = true;
			}
		}

		count++;
		if (columns > 0 && count % columns != 0 && i < mask.size() - 1)
		{
			if (isFilter && i == lastBit)
				continue;

			ImGui::SameLine();
		}
	}

	ImGui::Separator();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
}

void SceneTiles::DrawCombinations(const Tileset* tileset, float panelWidth)
{
	static bool isPopupOpen = false;
	static const int textureSize = 40;
	static const float itemSize = (textureSize + 8.0f) * 2.0f + 2.0f + 9.0f; // values from NeighbourCombo (padding & spacing)

	int columns = (int)(panelWidth / itemSize);

	int count = 0;
	for (int dir = 0; dir < 2; ++dir) //vertical & horizontal
	{
		for (unsigned int i = 0; i < tileset->GetSize(); ++i)
		{
			const Tile* tile = tileset->GetTile(i);
			const BitArray mask = tile->GetMasks()[dir];

			for (unsigned int j = 0; j < tileset->GetSize(); ++j)
			{
				if (!mask.getBit(j))
					continue;

				const Tile* neighbour = tileset->GetTile(j);

				String label = String("###%d%d", dir, i);
				bool hovered = (isPopupOpen && comboData.firstIndex == i && comboData.secondIndex == j && comboData.dir == dir);
				NeighbourCombo(label.c_str(), false, textureSize, tile->GetTexture(), neighbour->GetTexture(), dir, hovered);

				count++;
				if (count < columns)
					ImGui::SameLine();
				else
					count = 0;

				if (ImGui::OpenPopupOnItemClick("Neighbour Options"))
				{
					comboData.dir = dir;
					comboData.firstIndex = i;
					comboData.secondIndex = j;
				}
			}
		}
	}

	// Right Click
	if (ImGui::BeginPopup("Neighbour Options", ImGuiWindowFlags_NoMove))
	{
		if (ImGui::MenuItem("Remove"))
		{
			// Update Masks (both tiles in combo)
			int neighbourDir = NUM_NEIGHBOURS - comboData.dir - 1;
			App->event->Publish(new EventUpdateMask(comboData.firstIndex, comboData.dir, comboData.secondIndex, false));
			App->event->Publish(new EventUpdateMask(comboData.secondIndex, neighbourDir, comboData.firstIndex, false));

			isChanges = true;
			tileData[comboData.firstIndex].isChanged = true;
			tileData[comboData.secondIndex].isChanged = true;
		}
		ImGui::EndPopup();
		isPopupOpen = true;
	}
	else
		isPopupOpen = false;
}

bool SceneTiles::ToolbarIcon(unsigned int icon, const int buttonSize, const char* tooltip, bool selected)
{
	bool ret = false;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5, 0.5, 0.5, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5, 0.5, 0.5, 0.7f));

	if (selected)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.5, 0.5, 0.4f));

	if (ImGui::ImageButton((ImTextureID)icon, ImVec2(buttonSize, buttonSize)))
		ret = true;

	if (selected)
		ImGui::PopStyleColor();

	ImGui::PopStyleColor(3);

	// Tooltip
	ImGuiContext& g = *ImGui::GetCurrentContext();
	if (ImGui::IsItemHovered() && g.HoveredIdTimer > 0.8f)
	{
		ImGui::BeginTooltip();
		ImGui::Text(tooltip);
		ImGui::EndTooltip();
	}

	return ret;
}

// -----------------------------
bool SceneTiles::NeighbourCombo(const char* name, bool selected, float texSize, unsigned int tex1, unsigned int tex2, unsigned int orientation, bool rclick_hover)
{
	static const int rounding = 3.0f;
	static const int padding = 8.0f;
	static const int spacing = 2.0f;

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
	if (ImGui::IsItemHovered() || rclick_hover)
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

void SceneTiles::Shortcuts()
{
	// --- Shortcuts
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN) // Next Tile (Up Arrow)
	{
		currentTile++;
		if (currentTile >= tileData.size())
			currentTile = 0;
	}
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN) // Prev Tile (Down Arrow)
	{
		currentTile--;
		if (currentTile < 0)
			currentTile = tileData.size() - 1;
	}
}
