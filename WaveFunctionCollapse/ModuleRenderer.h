#pragma once
#include "Module.h"

#include "SDL/include/SDL.h"
#include <limits.h>

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

	const SDL_GLContext& GetContext() const { return context; }
	SDL_Renderer* GetRenderer() const { return renderer; }

	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;

private:
	SDL_GLContext context;
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;
	SDL_Rect viewport;
	SDL_Color background;
};
