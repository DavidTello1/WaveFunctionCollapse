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
	int limit = cells.size();

	while (!isCollapsed && limit > 0) {
		CollapseCells();
		limit--;
	}
}

void MapGenerator::Step()
{
	if (isCollapsed)
		return;

	CollapseCells();
}

void MapGenerator::Reset()
{
	isCollapsed = false;

	// Remove all cells
	for (unsigned int i = 0; i < cells.size(); ++i)
		delete cells[i];
	cells.clear();

	// Create new cells
	int numCells = width * height;
	for (int i = 0; i < numCells; ++i) {
		cells.push_back(new Cell(i, tiles.size()));
	}
}

void MapGenerator::DrawMap()
{
	static const SDL_Color black = { 0, 0, 0 };
	static const SDL_Color white = { 255, 255, 255 };
	static const SDL_Color red = { 255, 200, 200 };
	static const SDL_Color green = { 0, 255, 0 };

	static const int offsetX = (App->window->GetWidth() - (width * cellSize)) / 2;
	static const int offsetY = (App->window->GetHeight() - (height * cellSize)) / 2;

	static const int spacing = 2;

	for (unsigned int i = 0; i < cells.size(); ++i)
	{
		Cell* cell = cells[i];
		int x = cell->index % width;
		int y = cell->index / width;
		SDL_Rect rect = { x * cellSize + offsetX, y * cellSize + offsetY, cellSize, cellSize };

		if (isSpacedCells)
		{
			rect.x += spacing * x;
			rect.y += spacing * y;
		}

		if (isDrawTextures)
		{
			if (cell->isCollapsed)
			{
				Tile* tile = tiles[cell->tileID];
				App->renderer->Blit(tile->texture, rect.x, rect.y);
			}
			else
			{
				SDL_Color color = (cell->isInvalid) ? red : white;
				App->renderer->DrawQuad(rect, color.r, color.g, color.b, 255, cell->isInvalid);
			}
		}
		else
		{
			SDL_Color color = (cell->isInvalid) ? red : white;
			if (cell->isCollapsed && cell->tileID != 0)
				color = black;

			App->renderer->DrawQuad(rect, color.r, color.g, color.b, 255, cell->isInvalid || cell->isCollapsed);
		}
	}
}

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

void MapGenerator::CollapseCells()
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
