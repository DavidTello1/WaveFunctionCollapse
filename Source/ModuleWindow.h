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

	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }
	unsigned int GetZoom() const { return zoom; }

	void SetWidth(unsigned int width, bool update_size = true);
	void SetHeigth(unsigned int height, bool update_size = true);
	void SetSize(unsigned int width, unsigned int height, bool update_size = true);

private:
	SDL_Window* window = nullptr; //The window we'll be rendering to

	unsigned int width = DEFAULT_WIDTH;
	unsigned int height = DEFAULT_HEIGHT;
	unsigned int zoom = 1; //Scaling factor
};