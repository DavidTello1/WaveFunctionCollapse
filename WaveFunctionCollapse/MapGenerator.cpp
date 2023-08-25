#include "MapGenerator.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"

#include "Cell.h"
#include "Tile.h"

#include "PriorityQueue.h"

#include <random>
#include <time.h>

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
	srand(time(NULL));
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
		CollapseCell();
		limit--;
	}
}

void MapGenerator::Step()
{
	if (isFirstStep)
		FirstStep();

	if (isCollapsed)
		return;

	CollapseCell();
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
		unsigned int index = presetCells[i]->data;
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
	presetCells.add(index);
}

void MapGenerator::SetCell(unsigned int index, unsigned int tileID)
{
	if (index > cells.size() || tileID > tiles.size())
		return;

	cells[index]->SetCell(tileID);

	// Propagate
	List<int> queue = List<int>();
	queue.add(cells[index]->index);

	int limit = cells.size();
	while (!queue.empty() && limit > 0)
	{
		queue += PropagateCell(queue.front()->data);
		queue.pop_front();
		limit--;
	}
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
	PriorityQueue<Cell*> sortedCells = PriorityQueue<Cell*>(cells.size());
	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		if (cells[i]->isCollapsed || cells[i]->isInvalid)
			continue;

		sortedCells.push(cells[i]);
	}

	if (sortedCells.empty())
	{
		this->isCollapsed = true;
		return -1;
	}

	// Get list of cells with least entropy
	DynArray<int> possibleCells = DynArray<int>();
	int minEntropy = sortedCells.front()->GetEntropy();

	for (unsigned int i = 0; i < sortedCells.size(); ++i) 
	{
		Cell* cell = sortedCells[i];

		if (cell->GetEntropy() > minEntropy)
			break;

		possibleCells.push_back(cell->index);
	}

	// Pick cell randomly from list of possible cells
	int index = rand() % possibleCells.size();

	return possibleCells[index];
}

void MapGenerator::CollapseCell()
{
	int index = HeuristicPick();
	if (index == -1)
		return;

	cells[index]->Observe();

	if (cells[index]->tileID == -1)
		return;

	List<int> queue = List<int>();
	queue.add(cells[index]->index);

	int limit = cells.size();
	while (!queue.empty() && limit > 0)
	{
		queue += PropagateCell(queue.front()->data);
		queue.pop_front();
		limit--;
	}
}

List<int> MapGenerator::PropagateCell(const int index)
{
	List<int> list = List<int>();

	// Check neighbours
	for (int i = 0; i < NUM_NEIGHBOURS; ++i)
	{
		int neighbourIndex = CheckNeighbour(index, i);
		if (neighbourIndex == -1)
			continue;

		Cell* neighbour = cells[neighbourIndex];
		if (neighbour->isCollapsed)
			continue;

		BitMask prevMask = BitMask(*neighbour->mask);

		int tileIndex = cells[index]->tileID;
		Tile* tile = tiles[tileIndex];
		BitMask* cellMask = tile->masks[i];

		//LOG("--- prev neighbour mask:");
		//prevMask.Peek();

		//LOG("--- tile mask:");
		//cellMask->Peek();

		*neighbour->mask &= *cellMask; // Compare (bitwise And) both masks

		//LOG("--- prev neighbour mask after:");
		//prevMask.Peek();
		//LOG("--- neighbour mask:");
		//neighbour->mask->Peek();

		if (*neighbour->mask != prevMask)
		{
			neighbour->Update();
			if (neighbour->isCollapsed) //***
				list.add(neighbourIndex);
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
		unsigned int index = presetCells[i]->data;

		List<int> queue = List<int>();
		queue.add(cells[index]->index);

		int limit = cells.size();
		while (!queue.empty() && limit > 0)
		{
			queue += PropagateCell(queue.front()->data);
			queue.pop_front();
			limit--;
		}
	}

	isFirstStep = false;
}