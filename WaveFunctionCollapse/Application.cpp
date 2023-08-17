#include "Application.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleGUI.h"
#include "ModuleRenderer.h"

#include "mmgr/mmgr.h"

Application::Application()
{
	dt = 0.0f;
	frames = 0;
	last_frame_ms = -1;
	last_fps = -1;
	capped_ms = 0;
	fps_counter = 0;

	modules.push_back(input = new ModuleInput());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(resources = new ModuleResources());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(gui = new ModuleGUI());
	modules.push_back(renderer = new ModuleRenderer()); // renderer last to swap buffer
}

Application::~Application()
{
	// Release modules in reverse order
	for (auto it = modules.rbegin(); it != modules.rend(); ++it)
		RELEASE(*it);
	
	modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// We init everything, even if not enabled
	for (uint i = 0; i < modules.size() && ret; ++i)
		ret = modules[i]->Init();
	
	// If an error has occurred, we exit
	if (ret == false)
		return false;

	// Start all modules, only called for "active" modules
	for (uint i = 0; i < modules.size() && ret; ++i)
	{
		if (modules[i]->IsEnabled() == true)
			ret = modules[i]->Start();
	}

	return ret;
}

bool Application::Update()
{
	bool ret = true;
	PrepareUpdate();

	// --- Pre Update
	for (uint i = 0; i < modules.size() && ret; ++i)
		if (modules[i]->IsEnabled() == true)
			ret = modules[i]->PreUpdate(dt);

	// If an error has occurred, we exit
	if (ret == false)
		return false;

	// --- Main Update
	for (uint i = 0; i < modules.size() && ret; ++i)
		if (modules[i]->IsEnabled() == true)
			ret = modules[i]->Update(dt);

	// If an error has occurred, we exit
	if (ret == false)
		return false;

	// --- Post Update
	for (uint i = 0; i < modules.size() && ret; ++i)
		if (modules[i]->IsEnabled() == true)
			ret = modules[i]->PostUpdate(dt);

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (auto it = modules.rbegin(); it != modules.rend() && ret; ++it)
	{
		if ((*it)->IsEnabled() == true)
			ret = (*it)->CleanUp();
	}

	return ret;
}

void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

void Application::FinishUpdate()
{
	// Recap on framecount and fps
	++frames;
	++fps_counter;

	if (fps_timer.Read() >= 1000)
	{
		last_fps = fps_counter;
		fps_counter = 0;
		fps_timer.Start();
	}

	last_frame_ms = ms_timer.Read();

	// cap fps
	if (capped_ms > 0 && (last_frame_ms < capped_ms))
		SDL_Delay(capped_ms - last_frame_ms);
}

uint Application::GetFramerateLimit() const
{
	if (capped_ms > 0)
		return (uint)((1.0f / (float)capped_ms) * 1000.0f);
	
	return 0;
}

void Application::SetFramerateLimit(uint max_framerate)
{
	capped_ms = (max_framerate > 0) ? 1000 / max_framerate : 0;
}
