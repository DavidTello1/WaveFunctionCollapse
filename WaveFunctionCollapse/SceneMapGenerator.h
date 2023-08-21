#pragma once
#include "Scene.h"

#include "List.h"

class MapGenerator;

class SceneMapGenerator : public Scene
{
//private:
//	enum Flags {
//		IS_DRAW_TEXTURES = 1 << 1,
//		IS_DRAW_SPACED = 1 << 2
//	};

public:
	SceneMapGenerator();
	virtual ~SceneMapGenerator();

	bool Init() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	bool Draw() override;
	bool DrawUI() override;

private:
	MapGenerator* map = nullptr;

	// --- Map Data
	static const int numTiles = 7;
	static const int cellSize = 24;
	int width = 25;
	int height = 25;
	//Flags flags;

	// --- Debug
	List<unsigned int> selectedCells;

	bool isDrawTextures = true;
	bool isDrawSpaced = true;
	
	unsigned int stepIcon = 0;
	unsigned int playIcon = 0;
	unsigned int restartIcon = 0;
};

