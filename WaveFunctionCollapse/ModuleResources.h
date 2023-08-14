#pragma once
#include "Module.h"

#include "List.h"

struct SDL_Texture;
struct SDL_Surface;

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	virtual ~ModuleResources();

	bool Init() override;
	bool CleanUp() override;

	SDL_Texture* const	LoadTexture(const char* path);
	SDL_Texture* const LoadSurface(SDL_Surface* surface); // Translates a SDL_Surface to a SDL_Texture
	bool UnloadTexture(SDL_Texture* texture);
	void GetTextureSize(const SDL_Texture* texture, unsigned int& width, unsigned int& height) const;

private:
	List<SDL_Texture*> textures;
};
