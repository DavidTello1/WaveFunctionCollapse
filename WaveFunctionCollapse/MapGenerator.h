//#pragma once
//
//struct Tile {
//	int index = -1;
//
//	int* topMask = nullptr;
//	int* leftMask = nullptr;
//	int* rightMask = nullptr;
//	int* bottomMask = nullptr;
//};
//
//struct Cell {
//	int index = -1;
//	int* validTiles = nullptr;
//	bool isCollapsed = false;
//
//	int GetEntropy() { 
//		return this->validTiles.size(); 
//	}
//
//	void Update() { 
//		this->isCollapsed == (this->validTiles.size() == 1); 
//	}
//	
//	void Observe() {
//		int index = random(0, this->validTiles.size());
//		int* tile = this->validTiles[index];
//		this->validTiles.clear();
//		this->validTiles.push_back(tile);
//	}
//};
//
//class MapGenerator
//{
//public:
//	MapGenerator() {};
//	~MapGenerator() {};
//
//	void GenerateMap();
//	void DrawMap();
//
//private:
//	void Init();
//
//	int HeuristicPick();
//	void CollapseCells();
//	int* PropagateCells(Cell cell, int* list);
//	
//	int* CheckNeighbour(int* validTiles, int index, int dir);
//};
//
