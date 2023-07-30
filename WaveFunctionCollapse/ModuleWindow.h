#pragma once
#include "Module.h"

#define TITLE "Wave Function Collapse"
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

struct SDL_Window;

class ModuleWindow : public Module
{
public:
	ModuleWindow(bool start_enabled = true);
	virtual ~ModuleWindow();

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	SDL_Window* GetWindow() const { return window; }

	uint GetWidth() const { return width; }
	uint GetHeight() const { return height; }
	uint GetZoom() const { return zoom; }

	void SetWidth(uint width, bool update_size = true);
	void SetHeigth(uint height, bool update_size = true);
	void SetSize(uint width, uint height, bool update_size = true);

private:
	SDL_Window* window = nullptr; //The window we'll be rendering to

	uint width = DEFAULT_WIDTH;
	uint height = DEFAULT_HEIGHT;
	uint zoom = 1; //Scaling factor
};
