#pragma once
#include "Module.h"

class MapGenerator;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	virtual ~ModuleScene();

	bool Init() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

private:
	MapGenerator* map = nullptr;
};
