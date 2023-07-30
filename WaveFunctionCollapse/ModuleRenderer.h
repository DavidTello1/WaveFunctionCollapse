#pragma once
#include "Module.h"

#include "SDL2/include/SDL.h"

class ModuleRenderer : public Module
{
public:
	ModuleRenderer(bool start_enabled = true);
	virtual ~ModuleRenderer();

	bool Init() override;
	bool Start() override;
	bool PreUpdate(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	SDL_Renderer* GetRenderer() const { return renderer; }

	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;

private:
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;
	SDL_Rect viewport;
	SDL_Color background;
};
