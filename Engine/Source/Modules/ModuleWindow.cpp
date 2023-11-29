#include "ModuleWindow.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleEvent.h"

#include "SDL/include/SDL.h"
#include "mmgr/mmgr.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	window = NULL;
}

ModuleWindow::~ModuleWindow()
{
}

bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_DisplayMode DM;
	SDL_GetDesktopDisplayMode(0, &DM);

	// --- Create window
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	// Use OpenGL 4.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

	if (window == NULL)
	{
		LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// --- Events
	App->event->Subscribe(this, &ModuleWindow::OnResize);

	return true;
}

bool ModuleWindow::Start()
{
	// Force to trigger a chain of events to refresh aspect ratios	
	SDL_SetWindowSize(window, width, height);

	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if (window != nullptr)
		SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return true;
}

void ModuleWindow::SetWidth(unsigned int width, bool update_size)
{
	this->width = width;

	if (update_size && width > 0) 
		SDL_SetWindowSize(window, width, GetHeight());
}

void ModuleWindow::SetHeigth(unsigned int height, bool update_size)
{
	this->height = height;

	if (update_size && height > 0)
		SDL_SetWindowSize(window, GetWidth(), height);
}

void ModuleWindow::SetSize(unsigned int width, unsigned int height, bool update_size)
{
	this->width = width;
	this->height = height;

	if (update_size && width > 0 && height > 0)
		SDL_SetWindowSize(window, width, height);
}


void ModuleWindow::OnResize(EventWindowResize* e)
{
	SetSize(e->width, e->height, false);
}
