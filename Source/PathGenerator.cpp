#include "Globals.h"
#include "PathGenerator.h"

#include "MapGenerator.h"
#include "Pathfinding.h"
#include "Cell.h"
#include "Tile.h"
#include "RandomNumber.h"

#include <string>

PathGenerator::PathGenerator(MapGenerator* map) : map(map)
{
}

PathGenerator::~PathGenerator()
{
}

DynArray<int> PathGenerator::GeneratePaths()
{
    DynArray<int> final_map;

    int numCells = map->width * map->height;

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
    CarvePaths();

    // --- Final WFC
    FinishGeneration();

    return final_map;
}

void PathGenerator::Reset()
{
    areas.clear();

    walkabilityMap.clear();
    labelingMap.clear();
    costMap.clear();

    connections.clear();
    breadCrumbs.clear();

    paths.clear();
}

void PathGenerator::FindAreas()
{
    int numAreas = 1;
    std::map<int, int> equivalencies;

    for (int i = 0; i < labelingMap.size(); ++i)
    {
        Tile* tile = map->GetTileByID(map->cells[i]->tileID);
        
        // --- Get Cost Map
        costMap.push_back(tile->GetCost());

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
        for (int dir = 0; dir < 4; ++dir)
        {
            int neighbourIndex = map->CheckNeighbour(i, dir);

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
            neighbourAreas = neighbourAreas.flip();

            int areaLabel = neighbourAreas.front();

            if (neighbourAreas[j] == areaLabel)
                continue;

            connections[areaLabel].push_back(neighbourAreas[j]);
        }
    }

    // Remove duplicates
    for (auto it = connections.begin(); it != connections.end(); it++)
    {
        if (it->second.size() == 0)
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

void PathGenerator::CarvePaths()
{
    LOG("--- COST MAP ---")
    for (int i = 0; i < costMap.size(); ++i)
    {
        LOG("%d", costMap[i]);
    }

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

    // A* algorithm from startPos to points in each area

    // Reset Cells in path that are non-Walkable and all of its 8 neighbours
}

void PathGenerator::FinishGeneration()
{
    // Redo WFC algorithm for reseted cells and neighbours

}
