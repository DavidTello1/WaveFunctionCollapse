#pragma once
#include "Globals.h"
#include "Timer.h"

#include <vector> //***

class Module;
class ModuleInput;
class ModuleWindow;
class ModuleResources;
class ModuleScene;
class ModuleGUI;
class ModuleRenderer;

class Application
{
public:
	Application();
	virtual ~Application();

	bool Init();
	bool Update();
	bool CleanUp();

	int GetFPS() { return last_fps; }
	uint GetFramerateLimit() const;
	void SetFramerateLimit(uint max_framerate);

private:
	void PrepareUpdate();
	void FinishUpdate();

public:
	ModuleInput* input = nullptr;
	ModuleWindow* window = nullptr;
	ModuleResources* resources = nullptr;
	ModuleScene* scene = nullptr;
	ModuleGUI* gui = nullptr;
	ModuleRenderer* renderer = nullptr;

private:
	Timer	ms_timer;
	Timer	fps_timer;
	Uint32	frames;
	float	dt;
	int		fps_counter;
	int		last_frame_ms;
	int		last_fps;
	int		capped_ms;

	std::vector<Module*> modules;
};

extern Application* App;
