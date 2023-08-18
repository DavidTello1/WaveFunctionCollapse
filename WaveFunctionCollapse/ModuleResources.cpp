#include "ModuleResources.h"

#include "Application.h"
#include "ModuleRenderer.h"

#include "SDL_image/include/SDL_image.h"
#pragma comment(lib, "SDL_image/lib/x64/SDL2_image.lib")

#include "mmgr/mmgr.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init()
{
    LOG("Init Image library");

    // load support for the PNG image format
    int flags = IMG_INIT_PNG;
    int init = IMG_Init(flags);

    if ((init & flags) != flags)
    {
        LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
        return false;
    }

    return true;
}

bool ModuleResources::CleanUp()
{
    LOG("Freeing textures and Image library");

    ListItem<SDL_Texture*>* item;
    for (item = textures.front(); item != NULL; item = item->next)
    {
        SDL_DestroyTexture(item->data);
    }
    textures.clear();

    IMG_Quit();

    return true;
}

SDL_Texture* const ModuleResources::LoadTexture(const char* path)
{
    SDL_Texture* texture = NULL;
    SDL_Surface* surface = IMG_Load(path);

    if (surface == NULL)
    {
        LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
        return NULL;
    }

    texture = LoadSurface(surface);
    SDL_FreeSurface(surface);

    return texture;
}

SDL_Texture* const ModuleResources::LoadSurface(SDL_Surface* surface)
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(App->renderer->GetRenderer(), surface);

    if (texture == NULL)
    {
        LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
        return NULL;
    }

    textures.add(texture);
    return texture;
}

bool ModuleResources::UnloadTexture(SDL_Texture* texture)
{
    ListItem<SDL_Texture*>* item;
    for (item = textures.front(); item != NULL; item = item->next)
    {
        if (texture == item->data)
        {
            SDL_DestroyTexture(item->data);
            textures.erase(item);
            return true;
        }
    }

    return false;
}

void ModuleResources::GetTextureSize(const SDL_Texture* texture, unsigned int& width, unsigned int& height) const
{
    SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, (int*)&width, (int*)&height);
}
