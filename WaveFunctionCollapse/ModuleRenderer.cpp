#include "ModuleRenderer.h"

#include "Application.h"
#include "ModuleWindow.h"

#include "mmgr/mmgr.h"

ModuleRenderer::ModuleRenderer(bool start_enabled) : Module(start_enabled)
{
	camera = viewport = { 0, 0, 0, 0 };
	background = { 50, 50, 50, 255 };
}

ModuleRenderer::~ModuleRenderer()
{
}

bool ModuleRenderer::Init()
{
	LOG("Create SDL rendering context");

	Uint32 flags = SDL_RENDERER_ACCELERATED;
	renderer = SDL_CreateRenderer(App->window->GetWindow(), -1, flags);

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	//Get window surface and init camera
	SDL_Surface* windowSurface = SDL_GetWindowSurface(App->window->GetWindow());
	camera = { 0, 0, windowSurface->w, windowSurface->h };

	return true;
}

// Called before the first frame
bool ModuleRenderer::Start()
{
	LOG("render start");

	// back background
	SDL_RenderSetLogicalSize(renderer, App->window->GetWidth(), App->window->GetHeight());
	SDL_RenderGetViewport(renderer, &viewport);

	return true;
}

// Called each loop iteration
bool ModuleRenderer::PreUpdate(float dt)
{
	SDL_RenderClear(renderer);

	return true;
}

bool ModuleRenderer::PostUpdate(float dt)
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);

	return true;
}

// Called before quitting
bool ModuleRenderer::CleanUp()
{
	LOG("Destroying SDL render");

	SDL_DestroyRenderer(renderer);

	return true;
}

bool ModuleRenderer::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	uint zoom = App->window->GetZoom();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * zoom);
		rec.y = (int)(camera.y + rect.y * zoom);
		rec.w *= zoom;
		rec.h *= zoom;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool ModuleRenderer::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, SDL_RendererFlip flip, float speed, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;
	uint scale = App->window->GetZoom();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}
