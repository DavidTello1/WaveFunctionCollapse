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

    // --- Create Non-WalkableMask
    Init();

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

    // --- Last Checks
    CarvePaths();
    LastChecks();

    // --- Final WFC
    ResetNeighbours();
    FinishGeneration();

    // --- Fill Map with Objects, Portals & Traps
    FillMap();

    return final_map;
}

void PathGenerator::Step()
{
    if (step == 0)
    {
        int numCells = map->width * map->height;

        // --- Create Non-WalkableMask
        Init();

        // --- Connected Component Labeling
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
        LastChecks();
    }
    else if (step == 5)
    {
        ResetNeighbours();
    }
    else if (step == 6)
    {
        FinishGeneration();
    }
    else if (step == 7)
    {
        FillMap();
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

    portals.clear();
    traps.clear();
    items.clear();

    portals_exclusion.clear();
    traps_exclusion.clear();

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

void PathGenerator::Init()
{
    int numCells = map->width * map->height;
    labelingMap = DynArray<int>(numCells);
    labelingMap.fill(0);

    // Init mask of only non-walkable tiles (for FinishGeneration method)
    nonWalkableMask = BitArray(tiles_expanded.size());
    nonWalkableMask.setAll();
    for (int i = 0; i < tiles_expanded.size(); ++i)
    {
        if (tiles_expanded[i]->IsWalkable() == true)
            nonWalkableMask.clearBit(i);
    }

    // Init costMap and walkabilityMap
    for (int i = 0; i < numCells; ++i)
    {
        Tile* tile = map->GetTileByID(map->cells[i]->tileID);

        // --- Get Cost Map
        int cost = tile->GetCost();
        costMap.push_back(tile->GetCost());

        // --- Get Walkable Cells
        walkabilityMap.push_back(tile->IsWalkable());

        if (!tile->IsWalkable())
            continue;
    }

    // Exclusion Areas for FillMap
    portals_exclusion = DynArray<int>();
    traps_exclusion = DynArray<int>();

    DynArray<int> area = GetCellsInArea(7, 7, 13, 13);
    for (unsigned int i = 0; i < area.size(); ++i)
        portals_exclusion.push_back(area[i]);

    area = GetCellsInArea(10, 20, 7, 7);
    for (unsigned int i = 0; i < area.size(); ++i)
    {
        portals_exclusion.push_back(area[i]);
        traps_exclusion.push_back(area[i]);
    }

    area = GetCellsInArea(10, 10, 7, 7);
    for (unsigned int i = 0; i < area.size(); ++i)
        traps_exclusion.push_back(area[i]);
}

void PathGenerator::FindAreas()
{
    int numAreas = 1;
    std::map<int, int> equivalencies;

    for (int i = 0; i < walkabilityMap.size(); ++i)
    {        
        if (!walkabilityMap[i])
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
                //min = equivalencies[min];
                min = FindRoot(min, equivalencies);
            }

            if (equivalencies.find(max) != equivalencies.end()) // max label exists in dictionary
            {
                //max = equivalencies[max];
                max = FindRoot(max, equivalencies);

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
    Point2D startPos = Point2D(13, 26);
    Point2D endPos = Point2D(13, 15);
    int size = pathfinder.Propagate(startPos, endPos);
    if (size == 0)
        LOG("Error path not valid");

    paths.push_back(pathfinder.GetPath());
}

void PathGenerator::CarvePaths()
{
    // Set cells in path as walkable
    for (int i = 0; i < paths.size(); ++i)
    {
        DynArray<int> path = paths[i];

        for (int j = 0; j < path.size(); ++j)
        {
            int index = path[j];
            walkabilityMap[index] = true;
        }
    }
}

void PathGenerator::LastChecks()
{
    labelingMap.fill(0);
    areas.clear();

    FindAreas();
    CreateAreas();

    if (areas.size() > 1)
    {
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

            // Set Cells to non-walkable
            for (int i = 0; i < area.size(); ++i)
            {
                int cellIndex = area[i];
                walkabilityMap[cellIndex] = false;
            }
        }
    }

    // Change Tileset
    ChangeTileset(true);

    // Set all walkable cells with WalkableTile and all non-walkable cells with EmptyTile
    Tile* walkableTile = tiles_expanded.front();
    Tile* emptyTile = tiles_expanded[1];
    for (unsigned int i = 0; i < walkabilityMap.size(); ++i)
    {
        int id = (walkabilityMap[i] == true) ? walkableTile->GetID() : emptyTile->GetID();
        map->cells[i]->SetCell(id);
    }
}

void PathGenerator::ResetNeighbours()
{
    // Reset cells neighbouring walkable cells
    for (int i = 0; i < walkabilityMap.size(); ++i)
    {
        // Only check non-walkable cells
        if (walkabilityMap[i] == true)
            continue;

        // Reset if any neighbour is walkable
        for (int dir = 0; dir < 8; ++dir)
        {
            int neighbourIndex = CheckNeighbour(i, dir);
            if (neighbourIndex == -1)
                continue;

            if (walkabilityMap[neighbourIndex] == true)
            {
                Cell* cell = map->cells[i];

                cell->tileID = -1;
                cell->isCollapsed = false;
                cell->isInvalid = false;
                cell->mask = nonWalkableMask;

                map->numCollapsed--;
                break;
            }
        }
    }
}

void PathGenerator::FinishGeneration()
{
    int numCells = map->width * map->height;

    // Get all neighbours of reset cells
    for (int i = 0; i < numCells; ++i)
    {
        if (map->cells[i]->isCollapsed == true)
            map->PropagateCell(i);
    }
}

void PathGenerator::FillMap()
{
    PlaceItems();
    PlacePortals();
    PlaceTraps();
}

// -------------------------------------------------------
void PathGenerator::ChangeTileset(bool expanded)
{
    // Change Tileset
    map->tiles = (expanded) ? tiles_expanded : tiles;
}

int PathGenerator::CheckNeighbour(int index, int direction) const
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

int PathGenerator::FindRoot(int label, std::map<int, int>& equivalencies) const
{
    if (equivalencies.find(label) != equivalencies.end()) // label exists in dictionary
    {
        return FindRoot(equivalencies[label], equivalencies);
    }
    
    return label;
}

DynArray<int> PathGenerator::GetCellsInArea(int x, int y, int w, int h) const
{
    DynArray<int> area;

    for (int i = 0; i < w; ++i)
    {
        for (int j = 0; j < h; ++j)
        {
            int index = (x + i) + (y + j) * map->width;
            if (index < 0 || index > map->width * map->height)
                continue;

            area.push_back(index);
        }
    }

    return area;
}

// ---------------------------------
void PathGenerator::PlaceItems()
{
    // Dijkstra - Get highest-cost cell in each quadrant
    DynArray<int> candidates;
    
    // Sort quadrants in order (highest to lowest cost)
    

    // Choose if hidden room exists
    int itemCount = numItems;
    if (hiddenRoom_prob > map->RNG->GenerateBoundedInt(100))
    {
        if (candidates.empty())
            return;

        int index = candidates[0];

        int object_id = 0;
        items[index] = object_id;

        candidates.erase(0);
        itemCount--;
    }
        
    // Place Items (max 4, 3 if hidden room)
    for (unsigned int i = 0; i < itemCount; ++i)
    {
        if (candidates.empty())
            return;

        int result = map->RNG->GenerateBoundedInt(candidates.size());
        int index = candidates[result];

        //choice = map->RNG->GenerateBoundedInt(items_list.size()); //*** LIST OF ITEMS
        int object_id = 0; // items_list[choice];

        items[index] = object_id;
        candidates.erase(result);
    }
}

void PathGenerator::PlacePortals()
{
    // --- Get Candidates
    StaticArray<DynArray<int>, 4> quadrants; // 4 quadrants
    DynArray<DynArray<int>> candidates; // candidates by quadrants

    // TopLeft
    quadrants[0] = GetCellsInArea(
        0, 0, 
        ceil(map->width / 2.0f), floor(map->height / 2.0f)
    );

    // TopRight
    quadrants[1] = GetCellsInArea(
        ceil(map->width / 2.0f), 0,
        floor(map->width / 2.0f), ceil(map->height / 2.0f)
    );

    // BottomLeft
    quadrants[2] = GetCellsInArea(
        0, floor(map->height / 2.0f),
        floor(map->width / 2.0f), ceil(map->height / 2.0f)
    );

    // BottomRight
    quadrants[3] = GetCellsInArea(
        floor(map->width / 2.0f), ceil(map->height / 2.0f),
        ceil(map->width / 2.0f), floor(map->height / 2.0f)
    );


    for (int quadrant = 0; quadrant < 4; ++quadrant)
    {
        candidates.push_back(DynArray<int>()); // init candidates

        for (unsigned int i = 0; i < quadrants[quadrant].size(); ++i)
        {
            int index = quadrants[quadrant][i];

            if (walkabilityMap[index] == false)
                continue;

            int count = 0; // count of non-walkable neighbours
            for (unsigned int dir = 0; dir < NUM_NEIGHBOURS; dir++)
            {
                int neighbourIndex = map->CheckNeighbour(index, dir);
                if (neighbourIndex == -1)
                    continue;

                if (walkabilityMap[neighbourIndex] == true)
                    count++;
            }

            // Condition (cell is DeadEnd)
            if (count == 1)
            {
                // Check for excluded cells
                bool found = false;
                for (unsigned int j = 0; j < portals_exclusion.size(); ++j)
                {
                    if (portals_exclusion[j] == index)
                    {
                        found = true;
                        break;
                    }
                }

                if (found == false)
                    candidates[quadrant].push_back(index);
            }
        }
    }

    for (int i = 3; i >= 0; --i)
    {
        if (candidates[i].empty())
            candidates.erase(i);
    }
    
    // --- Select Cells to place Portals
    // 
    // 6 possible connections (TL-TR, TL-BL, TL-BR, TR-BL, TR-BR, BL-BR)
    DynArray<std::pair<int, int>> availConnections = DynArray<std::pair<int, int>>(6);
    for (int i = 0; i < 4; ++i)
    {
        if (candidates[i].empty())
            continue;

        for (int j = i + 1; j < 4; ++j)
        {
            if (candidates[j].empty())
                continue;

            availConnections.push_back({ i, j });
        }
    }


    // for each pair of portals
    for (unsigned int i = 0; i < numPortals; ++i)
    {
        if (availConnections.empty())
            return;

        // select combination randomly
        int result = map->RNG->GenerateBoundedInt(availConnections.size());

        int first_quadrant = availConnections[result].first;
        int second_quadrant = availConnections[result].second;

        // Select random cell from candidates in each of the two quadrants of the combination
        int res_1 = map->RNG->GenerateBoundedInt(candidates[first_quadrant].size());
        int res_2 = map->RNG->GenerateBoundedInt(candidates[second_quadrant].size());

        int first = candidates[first_quadrant][res_1];
        int second = candidates[second_quadrant][res_2];

        // Place Portal
        portals[first] = i;
        portals[second] = i;

        // Remove the combination from list of available connections
        availConnections.erase(result);

        // Remove from candidates
        candidates[first_quadrant].erase(res_1);
        candidates[second_quadrant].erase(res_2);
    }
}

void PathGenerator::PlaceTraps()
{
    // --- Get Candidates
    DynArray<int> candidates;
    for (unsigned int i = 0; i < walkabilityMap.size(); ++i)
    {
        if (walkabilityMap[i] == false)
            continue;

        int count = 0; // count of non-walkable neighbours
        for (unsigned int dir = 0; dir < NUM_NEIGHBOURS; dir++)
        {
            int neighbourIndex = map->CheckNeighbour(i, dir);
            if (neighbourIndex == -1)
                continue;

            if (walkabilityMap[neighbourIndex] == true)
                count++;
        }

        // Condition (cell is Corridor or Corner)
        if (count == 2)
        {
            // Check for excluded cells
            bool found = false;
            for (unsigned int j = 0; j < traps_exclusion.size(); ++j)
            {
                if (traps_exclusion[j] == i)
                {
                    found = true;
                    break;
                }
            }

            if (found == false)
                candidates.push_back(i);
        }
    }

    if (candidates.empty())
        return;

    // --- Select Cells to place Traps
    for (unsigned int i = 0; i < numTraps; ++i)
    {
        int result = map->RNG->GenerateBoundedInt(candidates.size());
        int index = candidates[result];

        //choice = map->RNG->GenerateBoundedInt(traps_list.size()); //*** LIST OF TRAPS (?)
        int trap_id = 0; // traps_list[choice];

        traps[index] = trap_id;
        candidates.erase(result);
    }
}
