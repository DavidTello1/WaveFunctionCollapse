#include "Globals.h"
#include "PathGenerator.h"

#include "MapGenerator.h"
#include "Cell.h"
#include "Tile.h"
#include "RandomNumber.h"

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

    GetAreas_FirstPass();
    GetAreas_SecondPass();

    // --- Delaunay Triangulation (BowyerWatson)
    ConnectAreas();

    // --- Prim's Algorithm
    GetPaths();

    // --- Dijkstra
    CarvePaths();

    // --- Final WFC
    FinishGeneration();

    return final_map;
}

void PathGenerator::Reset()
{
    areas.clear();
    walkableCells.clear();

    labelingMap.clear();
    equivalencies.clear();

    breadCrumbs.clear();
}

void PathGenerator::GetAreas_FirstPass()
{
    int numAreas = 1;

    for (int i = 0; i < labelingMap.size(); ++i)
    {
        // --- Get Walkable Cells
        Tile* tile = map->GetTileByID(map->cells[i]->tileID);
        if (!tile->IsWalkable())
            continue;
        else
            walkableCells.push_back(i);


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
}

void PathGenerator::GetAreas_SecondPass()
{
    // --- Join different labels from same area to single label
    for (int i = 0; i < walkableCells.size(); ++i)
    {
        int index = walkableCells[i];
        int label = labelingMap[index];

        bool containsLabel = true;
        while (containsLabel)
        {
            if (equivalencies.find(label) == equivalencies.end()) // if label not found
            {
                labelingMap[index] = label;
                break;
            }

            label = equivalencies[label]; // pseudo-recursivity
        }
    }

    // --- Create Areas
    // Create List of All Areas
    for (int i = 0; i < walkableCells.size(); ++i)
    {
        int index = walkableCells[i];
        int label = labelingMap[index];

        // Init label if it doesn't exist
        if (areas.find(label) == areas.end())
            areas[label] = DynArray<int>();

        // Add cell
        areas[label].push_back(index);
    }

    // Check for Area Size & Remove if too small
    for (auto it = areas.begin(); it != areas.end(); )
    {
        DynArray<int> area = it->second;

        if (area.size() >= MIN_AREA_SIZE)
        {
            ++it;
            continue;
        }

        // Remove Area
        it = areas.erase(it);
    }
}

// -----------------------------------
void PathGenerator::ConnectAreas()
{
    // Select random point from each area
    for (auto it = areas.begin(); it != areas.end(); it++)
    {
        DynArray<int> area = it->second;

        int index = map->RNG->GenerateBoundedInt(area.size());

        int cellIndex = area[index];

        breadCrumbs.push_back(cellIndex);
    }

    // Bowyer-Watson to generate Delaunay Triangulation
}

void PathGenerator::GetPaths()
{
    // Prim's Algorithm to select the main connections

    // Randomly select extra paths (weighted random)
}

void PathGenerator::CarvePaths()
{
    // A* algorithm from startPos to points in each area

    // Reset Cells in path that are non-Walkable and neighbours
}

void PathGenerator::FinishGeneration()
{
    // Redo WFC algorithm for reseted cells and neighbours

}
