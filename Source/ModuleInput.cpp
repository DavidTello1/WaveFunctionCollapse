#include "ModuleInput.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL.h"
#include "mmgr/mmgr.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouseButtons, KEY_IDLE, sizeof(KEY_STATE) * NUM_MOUSE_BUTTONS);
	close = false;
}

ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

bool ModuleInput::Init()
{
	LOG("Init SDL input event system");

	SDL_Init(0);
	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError(), 'e');
		return false;
	}

	return true;
}

bool ModuleInput::Start() 
{
	SDL_StopTextInput();

	return true;
}

// Called every draw update
bool ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();
	mouseMotionX = mouseMotionY = 0;

	// Check keyboard input
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			keyboard[i] = (keyboard[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT; 
		}
		else			  
		{ 
			keyboard[i] = (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE; 
		}
	}

	// Check mouse input
	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouseButtons[i] == KEY_DOWN)	
		{ 
			mouseButtons[i] = KEY_REPEAT; 
		}
		else if (mouseButtons[i] == KEY_UP)
		{ 
			mouseButtons[i] = KEY_IDLE;
		}
	}

	// Mouse events
	static SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			mouseButtons[e.button.button - 1] = KEY_DOWN;
			break;

		case SDL_MOUSEBUTTONUP:
			mouseButtons[e.button.button - 1] = KEY_UP;
			break;

		case SDL_MOUSEMOTION:
			mouseMotionX = e.motion.xrel;
			mouseMotionY = e.motion.yrel;
			mouseX = e.motion.x;
			mouseY = e.motion.y;
			break;

		case SDL_MOUSEWHEEL:
			mouseWheel = e.wheel.y;
			App->event->Publish(new EventCameraZoom(mouseWheel));
			break;


		case SDL_WINDOWEVENT:
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				App->event->Publish(new EventWindowResize(e.window.data1, e.window.data2));
				break;
			}
			break;

		case SDL_QUIT:
			close = true;
			break;
		}
	}
	return true;
}

bool ModuleInput::PostUpdate(float dt)
{
	return !close;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");

	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	return true;
}
