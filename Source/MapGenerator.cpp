#include "MapGenerator.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"

#include "Cell.h"
#include "Tile.h"

#include "RandomNumber.h"
//#include "PriorityQueue.h"
#include <queue> //***

#include "mmgr/mmgr.h"

MapGenerator::MapGenerator(const int width, const int height, const int cellSize, const DynArray<Tile*>& tiles)
{
	this->width = width;
	this->height = height;
	this->cellSize = cellSize;

	// Init Tiles Array
	for (unsigned int i = 0; i < tiles.size(); ++i)
	{
		this->tiles.push_back(tiles[i]);
	}

	// Init Cells Array
	int numCells = width * height;
	for (int i = 0; i < numCells; ++i) {
		cells.push_back(new Cell(i, this->tiles.size()));
	}

	// Seed random
	RNG = RandomNumber();
}

MapGenerator::~MapGenerator()
{
	for (unsigned int i = 0; i < cells.size(); ++i)
		delete cells[i];
	cells.clear();

	for (unsigned int i = 0; i < tiles.size(); ++i)
		delete tiles[i];
	tiles.clear();
}

void MapGenerator::GenerateMap()
{
	if (isFirstStep)
		FirstStep();

	int limit = cells.size();
	while (!isCollapsed && limit > 0) {

		int index = HeuristicPick();
		if (index == -1)
			continue;

		CollapseCell(index);
		PropagateCell(index);

		limit--;
	}
}

void MapGenerator::Step()
{
	if (isCollapsed)
		return;

	if (isFirstStep)
		FirstStep();

	int index = HeuristicPick();
	if (index == -1)
		return;

	CollapseCell(index);
	PropagateCell(index);
}

void MapGenerator::ResetMap()
{
	isFirstStep = true;
	isCollapsed = false;

	// Reset all not preset cells
	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		if (!cells[i]->isPreset)
			cells[i]->Reset();
	}
}

void MapGenerator::Resize()
{
	// Remove cells
	for (unsigned int i = 0; i < cells.size(); ++i)
		delete cells[i];
	cells.clear();

	// Create new cell array with updated size
	int numCells = width * height;
	for (int i = 0; i < numCells; ++i) {
		cells.push_back(new Cell(i, this->tiles.size()));
	}
}

void MapGenerator::ClearPresetCells()
{
	for (unsigned int i = 0; i < presetCells.size(); ++i)
	{
		unsigned int index = presetCells.at(i);
		cells[index]->Reset();
	}
	presetCells.clear();
}

void MapGenerator::PresetCell(unsigned int index, unsigned int tileID)
{
	if (index > cells.size() || tileID > tiles.size())
		return;

	cells[index]->SetCell(tileID);
	cells[index]->isPreset = true;
	presetCells.append(index);
}

void MapGenerator::SetCell(unsigned int index, unsigned int tileID)
{
	if (index > cells.size() || tileID > tiles.size())
		return;

	cells[index]->SetCell(tileID);
	PropagateCell(index);
}

void MapGenerator::ResetCell(unsigned int index)
{
	cells[index]->Reset();
	presetCells.erase(index);
}

//------------------------------------------------
int MapGenerator::HeuristicPick()
{
	// Cells list sorted by entropy (smaller entropy first)
	List<Cell*> possibleCells;
	int minEntropy = cells.front()->mask.size();
	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		Cell* cell = cells[i];
		if (cell->isCollapsed || cell->isInvalid || cell->GetEntropy() > minEntropy)
			continue;

		if (cell->GetEntropy() < minEntropy)
		{
			possibleCells.clear();
			minEntropy = cell->GetEntropy();
		}

		possibleCells.append(cell);
	}

	if (possibleCells.empty())
	{
		this->isCollapsed = true;
		return -1;
	}

	for (unsigned int i = 0; i < possibleCells.size(); ++i)
	{
		Cell* cell = possibleCells.at(i);
		LOG(" - %d: %d", cell->index, cell->GetEntropy());
	}
	LOG("----------------------")


	// Pick cell randomly from list of possible cells
	int index = RNG.GenerateBoundedInt(possibleCells.size());

	return possibleCells.at(index)->index;
}

void MapGenerator::CollapseCell(unsigned int index)
{
	// Fill array with possible tiles to choose from
	DynArray<int> possibleTiles;
	for (unsigned int i = 0; i < cells[index]->mask.size(); ++i)
	{
		if (cells[index]->mask.getBit(i))
			possibleTiles.push_back(i);
	}

	if (possibleTiles.empty())
	{
		Cell* cell = cells[index]; //*** for testing purposes
		return;
	}

	// Pick a tile from array
	uint32_t tile = RNG.GenerateBoundedInt(possibleTiles.size());
	int tileID = possibleTiles[tile];

	// Set Cell to Tile
	cells[index]->Observe(tileID);
}

void MapGenerator::PropagateCell(unsigned int index)
{
	List<int> queue = List<int>();
	queue.append(cells[index]->index);

	int limit = cells.size();
	while (!queue.empty() && limit > 0)
	{
		int cellIndex = queue.front();
		queue.append(PropagateNeighbours(cellIndex));
		queue.pop_front();
		limit--;
	}
}

List<int> MapGenerator::PropagateNeighbours(unsigned int index) //***
{
	List<int> list;

	for (int i = 0; i < NUM_NEIGHBOURS; ++i)
	{
		int neighbourIndex = CheckNeighbour(index, i);
		if (neighbourIndex == -1)
			continue;

		Cell* neighbour = cells[neighbourIndex];
		if (neighbour->isCollapsed)
			continue;

		BitArray prevMask = BitArray(neighbour->mask);

		if (cells[index]->isCollapsed)
		{
			int tileIndex = cells[index]->tileID;
			Tile* tile = tiles[tileIndex];
			BitArray cellMask = tile->masks[i];

			neighbour->mask &= cellMask; // Compare (bitwise And) both masks
		}
		//else
		//{
		//	List<unsigned int> setBits = cells[index]->mask->GetSetBits();

		//	for (unsigned int j = 0; j < setBits.size(); ++j)
		//	{
		//		int tileIndex = setBits[j]->data;
		//		Tile* tile = tiles[tileIndex];
		//		BitArray* cellMask = tile->masks[i];

		//		*neighbour->mask &= *cellMask; // Compare (bitwise Or) both masks
		//	}
		//}

		if (neighbour->mask != prevMask)
		{
			list.append(neighbourIndex);

			if (neighbour->mask.count() == 1)
			{
				for (unsigned int i = 0; i < neighbour->mask.size(); ++i) //*** BitArray->FirstSetBit()
				{
					if (neighbour->mask.getBit(i))
						neighbour->Observe(i);
				}
			}
			else if (neighbour->mask.count() == 0)
			{
				neighbour->isInvalid = true;
			}
		}
	}

	return list;
}

int MapGenerator::CheckNeighbour(int index, int direction) //*** change to x, y 
{
	if (index < 0 || index >= (int)cells.size())
		return -1;

	switch (direction)
	{
	case 0: // TopLeft
		if ((index - 1) > width)
			return index - 1 - width;
		break;

	case 1: // Top
		if (index > width)
			return index - width;
		break;

	case 2: // TopRight
		if ((index + 1) > width)
			return index + 1 - width;
		break;

	case 3: // Left
		if (index % width != 0)
			return index - 1;
		break;

	case 4: // Right
		if ((index + 1) % width != 0)
			return index + 1;
		break;

	case 5: // BottomLeft
		if ((index - 1) < width * (height - 1))
			return index - 1 + width;
		break;

	case 6: // Bottom
		if (index < width * (height - 1))
			return index + width;
		break;

	case 7: // BottomRight
		if ((index + 1) < width * (height - 1))
			return index + 1 + width;
		break;
	}

	return -1;
}

void MapGenerator::FirstStep()
{
	for (unsigned int i = 0; i < presetCells.size(); ++i)
	{
		unsigned int index = presetCells.at(i);
		PropagateCell(index);
	}

	isFirstStep = false;
}