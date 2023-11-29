#include "MapGenerator.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"

#include "Cell.h"
#include "Tile.h"

#include "RandomNumber.h"

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
	presetCells.clear();

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
	while (!IsFinished() && limit > 0) {

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
	if (IsFinished())
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
	numCollapsed = 0;

	// Reset all not preset cells
	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		if (!cells[i]->isPreset)
			cells[i]->Reset();
	}
}

void MapGenerator::Resize()
{
	// Clear preset cells list
	presetCells.clear();

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
	// Get Cells with minimum entropy
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

	// If no possible cells -> Map is Collapsed
	if (possibleCells.empty())
	{
		numCollapsed = cells.size();
		return -1;
	}

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
		return;

	// Pick a tile from array
	uint32_t tile = RNG.GenerateBoundedInt(possibleTiles.size());
	int tileID = possibleTiles[tile];

	// Set Cell to Tile
	cells[index]->Observe(tileID);
	numCollapsed++;
}

void MapGenerator::PropagateCell(unsigned int index)
{
	List<int> queue;
	queue.append(cells[index]->index);

	int limit = cells.size();
	while (!queue.empty() && limit > 0)
	{
		int cellIndex = queue.front();
		List<int> neighbours = PropagateNeighbours(cellIndex);

		queue.append(neighbours);
		queue.pop_front();

		limit--;
	}
}

List<int> MapGenerator::PropagateNeighbours(unsigned int index)
{
	List<int> list;

	for (int dir = 0; dir < NUM_NEIGHBOURS; ++dir)
	{
		int neighbourIndex = CheckNeighbour(index, dir);
		if (neighbourIndex == -1)
			continue;

		Cell* neighbour = cells[neighbourIndex];
		if (neighbour->isCollapsed)
			continue;

		BitArray prevMask = BitArray(neighbour->mask);

		Cell* cell = cells[index];
		if (cell->isCollapsed)
		{
			int tileIndex = cell->tileID;
			Tile* tile = tiles[tileIndex];
			BitArray tileMask = tile->GetMasks()[dir];

			neighbour->mask &= tileMask; // Compare (bitwise And) both masks
		}
		else
		{
			BitArray accMask = BitArray(tiles.size()); // All bits set to 0

			for (unsigned int tileIndex = 0; tileIndex < tiles.size(); ++tileIndex)
			{
				if (cell->mask.getBit(tileIndex))
				{
					Tile* tile = tiles[tileIndex];
					BitArray tileMask = tile->GetMasks()[dir];

					accMask |= tileMask;
				}
			}

			neighbour->mask &= accMask;
		}

		if (neighbour->mask != prevMask)
		{
			if (neighbour->mask.count() == 0)
			{
				neighbour->isInvalid = true;
				continue;
			}

			if (neighbour->mask.count() == 1)
			{
				CollapseCell(neighbourIndex);
			}

			list.append(neighbourIndex);
		}
	}

	return list;
}

int MapGenerator::CheckNeighbour(int index, int direction)
{
	if (index < 0 || index >= (int)cells.size())
		return -1;

	switch (direction)
	{
	case 0: // Top
		if (index >= width)
			return index - width;
		break;

	case 1: // Left
		if (index % width != 0)
			return index - 1;
		break;

	case 2: // Right
		if ((index + 1) % width != 0)
			return index + 1;
		break;

	case 3: // Bottom
		if (index < width * (height - 1))
			return index + width;
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