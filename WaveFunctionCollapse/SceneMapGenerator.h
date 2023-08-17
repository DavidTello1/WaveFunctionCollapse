#pragma once
#include "Scene.h"

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

	bool DrawUI() override;

private:
	static const int cellSize = 24;
	int width = 25;
	int height = 25;
	//Flags flags;

	MapGenerator* map = nullptr;
};

