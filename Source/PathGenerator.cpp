#include "Globals.h"
#include "PathGenerator.h"

#include "MapGenerator.h"
#include "Pathfinding.h"
#include "Cell.h"
#include "Tile.h"
#include "RandomNumber.h"

#include "String.h"
#include "Utils.h"
#include <string>

#include "mmgr/mmgr.h"

PathGenerator::PathGenerator(MapGenerator* map) : map(map)
{
}

PathGenerator::~PathGenerator()
{
    for (unsigned int i = 0; i < tiles_expanded.size(); ++i)
        delete tiles_expanded[i];
    tiles_expanded.clear();
}

DynArray<int> PathGenerator::GeneratePaths()
{
    DynArray<int> final_map;

    int numCells = map->width * map->height;
    // --- Create WalkableMask
    CreateWalkableMask();

    // --- Connected Component Labeling
    labelingMap = DynArray<int>(numCells);
    labelingMap.fill(0);
    FindAreas();

    // --- Init Areas' map and Remove small ones
    CreateAreas();
    RemoveAreas(MIN_AREA_SIZE);

    // --- Get neighbouring areas to connect to
    GetConnections();

    // --- Pathfinding
    SetBreadCrumbs();
    CalcPaths();

    // --- Final WFC
    CarvePaths();
    FinishGeneration();

    return final_map;
}

void PathGenerator::Step()
{
    if (step == 0)
    {
        int numCells = map->width * map->height;

        // --- Create WalkableMask
        CreateWalkableMask();

        // --- Connected Component Labeling
        labelingMap = DynArray<int>(numCells);
        labelingMap.fill(0);
        FindAreas();
        CreateAreas();
    }
    else if (step == 1)
    {
        RemoveAreas(MIN_AREA_SIZE);
    }
    else if (step == 2)
    {
        GetConnections();
        SetBreadCrumbs();
    }
    else if (step == 3)
    {
        CalcPaths();
    }
    else if (step == 4)
    {
        CarvePaths();
    }
    else if (step == 5)
    {
        FinishGeneration();
    }
    else if (step == 6)
    {
        LastChecks();
    }
    
    step++;
}

void PathGenerator::Reset()
{
    ChangeTileset(false);
 
    areas.clear();

    walkabilityMap.clear();
    labelingMap.clear();
    costMap.clear();

    connections.clear();
    breadCrumbs.clear();

    paths.clear();

    step = 0;
}

void PathGenerator::SetTileset(const DynArray<Tile*>& tileset)
{
    // --- Copy WFC tiles
    tiles.clear();
    for (unsigned int i = 0; i < map->tiles.size(); ++i)
        tiles.push_back(map->tiles[i]);

    // --- Expanded Tileset
    // Remove Tiles
    for (unsigned int i = 0; i < tiles_expanded.size(); ++i)
        delete tiles_expanded[i];
    tiles_expanded.clear();

    // Init Tiles Array
    for (unsigned int i = 0; i < tileset.size(); ++i)
    {
        tiles_expanded.push_back(new Tile(tileset[i]));
    }
}

void PathGenerator::FindAreas()
{
    int numAreas = 1;
    std::map<int, int> equivalencies;

    for (int i = 0; i < labelingMap.size(); ++i)
    {
        Tile* tile = map->GetTileByID(map->cells[i]->tileID);
        
        // --- Get Cost Map
        int cost = tile->GetCost();
        if (map->cells[i]->isPreset && tile->IsWalkable() == false)
            cost *= 100;
        costMap.push_back(cost);

        // --- Get Walkable Cells
        walkabilityMap.push_back(tile->IsWalkable());

        if (!tile->IsWalkable())
            continue;


        // --- Connected Component Labeling (First Pass)

        // Get Top & Left Neighbours
        // If both == 0 -> add to labelingMap (dictionary)
        // If only one != 0 -> add to map with neighbour's label
        // If both != 0 -> add to map with neighbour_1's label & add both neighbours' labels to equivalencyList

        // Neighbours (Top & Left)
        int top = 0;
        if (i >= map->width)
            top = labelingMap[i - map->width];

        int left = 0;
        if (i % map->width != 0)
            left = labelingMap[i - 1];

        if (top == 0 && left == 0) // None are set
        {
            labelingMap[i] = numAreas;
            numAreas++;
        }
        else if (top == 0 && left != 0) // Only one is set
        {
            labelingMap[i] = left;
        }
        else if (top != 0 && left == 0)
        {
            labelingMap[i] = top;
        }
        else // Both are set
        {
            if (top == left)
            {
                labelingMap[i] = top;
                continue;
            }

            int min = MIN(left, top);
            int max = MAX(left, top);

            labelingMap[i] = min;

            if (equivalencies.find(min) != equivalencies.end()) // min label exists in dictionary
            {
                min = equivalencies[min];
            }

            if (equivalencies.find(max) != equivalencies.end()) // max label exists in dictionary
            {
                max = equivalencies[max];

                if (min != max)
                {
                    int min2 = MIN(min, max);
                    int max2 = MAX(min, max);

                    if (equivalencies.find(max2) == equivalencies.end())
                        equivalencies[max2] = min2;
                }
            }
            else // !Contains(max)
            {
                equivalencies[max] = min;
            }
        }
    }

    // --- Join different labels from same area to single label
    for (int i = 0; i < walkabilityMap.size(); ++i)
    {
        if (walkabilityMap[i] == false)
            continue;

        int label = labelingMap[i];

        bool containsLabel = true;
        while (containsLabel)
        {
            if (equivalencies.find(label) == equivalencies.end()) // if label not found
            {
                labelingMap[i] = label;
                break;
            }

            label = equivalencies[label]; // pseudo-recursivity
        }
    }
}

void PathGenerator::CreateAreas()
{
    // Create List of All Areas
    for (int i = 0; i < walkabilityMap.size(); ++i)
    {
        if (walkabilityMap[i] == false)
            continue;

        int label = labelingMap[i];

        // Init label if it doesn't exist
        if (areas.find(label) == areas.end())
            areas[label] = DynArray<int>();

        // Add cell
        areas[label].push_back(i);
    }
}

void PathGenerator::RemoveAreas(int minSize)
{
    // Check for Area Size & Remove if too small
    for (auto it = areas.begin(); it != areas.end(); )
    {
        DynArray<int> area = it->second;

        if (area.size() >= minSize)
        {
            ++it;
            continue;
        }

        // Set Cells to Blocked Tile (non-walkable)
        for (int i = 0; i < area.size(); ++i)
        {
            int cellIndex = area[i];
            const Tile* tile = map->GetAllTiles().front(); // blocked Tile

            // Set cell's tileID to BlockedTile
            map->SetCell(cellIndex, tile->GetID());

            // Update Walkability Map
            walkabilityMap[cellIndex] = false;

            // Update Cost Map
            costMap[cellIndex] = tile->GetCost();
        }

        // Remove Area
        it = areas.erase(it);
    }
}

// -----------------------------------
void PathGenerator::GetConnections()
{
    // Init Connections Map
    for (auto it = areas.begin(); it != areas.end(); it++)
        connections[it->first] = DynArray<int>();


    // Get Neighbouring Areas
    for (int i = 0; i < walkabilityMap.size(); ++i)
    {
        if (walkabilityMap[i] == true) //only check nonWalkable cells
            continue;

        DynArray<int> neighbourAreas;

        // Check neighbours
        for (int dir = 0; dir < 8; ++dir)
        {
            int neighbourIndex = CheckNeighbour(i, dir);

            if (neighbourIndex == -1 || walkabilityMap[neighbourIndex] == false)
                continue;

            neighbourAreas.push_back(labelingMap[neighbourIndex]);
        }

        // Check if areas are different (there is a connection)
        if (neighbourAreas.size() <= 1)
            continue;

        // Add neighbours
        for (int j = 1; j < neighbourAreas.size(); ++j)
        {
            neighbourAreas = neighbourAreas.sort();
            neighbourAreas = neighbourAreas.flip(); // smaller values first

            int areaLabel = neighbourAreas.front();

            if (neighbourAreas[j] == areaLabel)
                continue;

            connections[areaLabel].push_back(neighbourAreas[j]);
        }
    }

    // Remove duplicates
    for (auto it = connections.begin(); it != connections.end(); it++)
    {
        if (it->second.empty())
            continue;

        it->second = it->second.sort();

        int index = 0;
        for (int i = 0; i < it->second.size() - 1; ++i)
        {
            if (it->second[i] != it->second[i + 1])
                it->second[index++] = it->second[i];

        }
        it->second[index++] = it->second[it->second.size() - 1];
        it->second.resize(index);
    }
}

void PathGenerator::SetBreadCrumbs()
{
    // Select random point from each area
    for (auto it = areas.begin(); it != areas.end(); it++)
    {
        DynArray<int> area = it->second;

        int index = map->RNG->GenerateBoundedInt(area.size());

        breadCrumbs[it->first] = area[index];
    }
}

void PathGenerator::CalcPaths()
{
    Pathfinding pathfinder = Pathfinding(map->width, map->height, costMap);

    for (auto it = connections.begin(); it != connections.end(); it++)
    {
        int origin_index = breadCrumbs[it->first];

        Point2D origin = Point2D(origin_index % map->width, origin_index / map->width);

        DynArray<int> connectedAreas = it->second;
        for (int i = 0; i < connectedAreas.size(); ++i)
        {
            int dest_index = breadCrumbs[connectedAreas[i]];

            Point2D destination = Point2D(dest_index % map->width, dest_index / map->width);

            int size = pathfinder.Propagate(origin, destination);
            if (size == 0)
                LOG("Error path not valid");

            paths.push_back(pathfinder.GetPath());
        }
    }

    // Start to End is walkable
    Point2D startPos = Point2D(12, 25);
    Point2D endPos = Point2D(12, 14);
    int size = pathfinder.Propagate(startPos, endPos);
    if (size == 0)
        LOG("Error path not valid");

    paths.push_back(pathfinder.GetPath());
}

void PathGenerator::CarvePaths() //*** NEEDS OPTIMIZATION (when doing pathfinding save nonWalkable cells in array instead of traversing all paths)
{
    // Reset Cells in path that are non-Walkable and its neighbours

    for (int i = 0; i < paths.size(); ++i)
    {
        DynArray<int> path = paths[i];

        for (int j = 0; j < path.size(); ++j)
        {
            int index = path[j];

            Cell* cell = map->GetCell(index);
            if (cell->isCollapsed == false)
                continue;

            const Tile* tile = map->GetTileByID(cell->tileID);

            // Reset Cell if it is a wall
            if (tile->IsWalkable())
                continue;

            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;
            
            map->numCollapsed--;


            // Reset walkable neighbours (8 directions)
            ResetNeighbours(index);
        }
    }
}

void PathGenerator::LastChecks()
{
    labelingMap.fill(0);
    areas.clear();
    walkabilityMap.clear();
    costMap.clear();

    FindAreas();
    CreateAreas();

    if (areas.size() == 1)
        return;

    int biggestArea = 0;
    int size = 0;
    for (auto it = areas.begin(); it != areas.end(); it++)
    {
        if (it->second.size() > size)
        {
            size = it->second.size();
            biggestArea = it->first;
        }
    }

    for (auto it = areas.begin(); it != areas.end(); it++)
    {
        if (it->first == biggestArea)
            continue;

        DynArray<int> area = it->second;

        // Set Cells to Blocked Tile (non-walkable)
        for (int i = 0; i < area.size(); ++i)
        {
            int cellIndex = area[i];
            const Tile* tile = map->GetAllTiles().front(); // blocked Tile

            map->SetCell(cellIndex, tile->GetID());
            walkabilityMap[cellIndex] = false;
        }
    }
}

void PathGenerator::FinishGeneration()
{
    DynArray<int> resetNeighbours;
    int numCells = map->width * map->height;

    // Get all neighbours of reset cells
    for (int i = 0; i < numCells; ++i)
    {
        Cell* cell = map->GetCell(i);

        if (cell->isCollapsed == false)
            continue;

        // if cell is neighbour of a reset cell, propagate it
        for (int dir = 0; dir < 4; ++dir)
        {
            int neighbourIndex = map->CheckNeighbour(i, dir);
            if (neighbourIndex == -1)
                continue;

            Cell* neighbour = map->GetCell(neighbourIndex);

            if (neighbour->isCollapsed == false)
            {
                resetNeighbours.push_back(i);
                break;
            }
        }
    }


    // Redo WFC algorithm for reseted cells and neighbours
    ChangeTileset(true);
    for (unsigned int i = 0; i < resetNeighbours.size(); ++i)
    {
        int index = resetNeighbours[i];
        map->PropagateCell(index);
    }
}

void PathGenerator::CreateWalkableMask()
{
    // Init mask of only walkable tiles (for carvePaths method)
    walkableMask = BitArray(tiles_expanded.size());
    walkableMask.setAll();
    for (int i = 0; i < tiles_expanded.size(); ++i)
    {
        if (tiles_expanded[i]->IsWalkable() == false)
            walkableMask.clearBit(i);
    }
}

void PathGenerator::ResetNeighbours(int index)
{
    if (index < 0 || index >= map->width * map->height)
        return;

    // Top
    if (index >= map->width)
    {
        int top = index - map->width;
        Cell* cell = map->GetCell(top);

        if (cell->isCollapsed && map->GetTileByID(cell->tileID)->IsWalkable())
        {
            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;

            map->numCollapsed--;
        }
    }

    // Left
    if (index % map->width != 0)
    {
        int left = index - 1;
        Cell* cell = map->GetCell(left);

        if (cell->isCollapsed && map->GetTileByID(cell->tileID)->IsWalkable())
        {
            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;

            map->numCollapsed--;
        }
    }

    // Right
    if ((index + 1) % map->width != 0)
    {
        int right = index + 1;
        Cell* cell = map->GetCell(right);

        if (cell->isCollapsed && map->GetTileByID(cell->tileID)->IsWalkable())
        {
            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;

            map->numCollapsed--;
        }
    }

    // Bottom
    if (index < map->width * (map->height - 1))
    {
        int bottom = index + map->width;
        Cell* cell = map->GetCell(bottom);

        if (cell->isCollapsed && map->GetTileByID(cell->tileID)->IsWalkable())
        {
            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;

            map->numCollapsed--;
        }
    }

    // TopLeft
    if (index >= map->width && index % map->width != 0)
    {
        int topLeft = index - map->width - 1;
        Cell* cell = map->GetCell(topLeft);

        if (cell->isCollapsed && map->GetTileByID(cell->tileID)->IsWalkable())
        {
            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;

            map->numCollapsed--;
        }
    }

    // TopRight
    if (index >= map->width && (index + 1) % map->width != 0)
    {
        int topRight = index - map->width + 1;
        Cell* cell = map->GetCell(topRight);

        if (cell->isCollapsed && map->GetTileByID(cell->tileID)->IsWalkable())
        {
            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;

            map->numCollapsed--;
        }
    }

    // BottomLeft
    if (index < map->width * (map->height - 1) && index % map->width != 0)
    {
        int bottomLeft = index + map->width - 1;
        Cell* cell = map->GetCell(bottomLeft);

        if (cell->isCollapsed && map->GetTileByID(cell->tileID)->IsWalkable())
        {
            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;

            map->numCollapsed--;
        }
    }

    // BottomRight
    if (index < map->width * (map->height - 1) && (index + 1) % map->width != 0)
    {
        int bottomRight = index + map->width + 1;
        Cell* cell = map->GetCell(bottomRight);

        if (cell->isCollapsed && map->GetTileByID(cell->tileID)->IsWalkable())
        {
            cell->tileID = -1;
            cell->isCollapsed = false;
            cell->isInvalid = false;
            cell->mask = walkableMask;

            map->numCollapsed--;
        }
    }
}

void PathGenerator::ChangeTileset(bool expanded)
{
    // Change Tileset
    map->tiles = (expanded) ? tiles_expanded : tiles;
}

int PathGenerator::CheckNeighbour(int index, int direction)
{
    if (index < 0 || index >= (int)map->cells.size())
        return -1;

    switch (direction)
    {
    case 0: // Top
        if (index >= map->width)
            return index - map->width;
        break;

    case 1: // Left
        if (index % map->width != 0)
            return index - 1;
        break;

    case 2: // Right
        if ((index + 1) % map->width != 0)
            return index + 1;
        break;

    case 3: // Bottom
        if (index < map->width * (map->height - 1))
            return index + map->width;
        break;

    case 4: // TopLeft
        if (index >= map->width && index % map->width != 0)
            return index - map->width -1;
        break;

    case 5: // TopRight
        if (index >= map->width && (index + 1) % map->width != 0)
            return index - map->width + 1;
        break;

    case 6: // BottomLeft
        if (index < map->width * (map->height - 1) && index % map->width != 0)
            return index + map->width - 1;
        break;

    case 7: // BottomRight
        if (index < map->width * (map->height - 1) && (index + 1) % map->width != 0)
            return index + map->width + 1;
        break;
    }

    return -1;
}
