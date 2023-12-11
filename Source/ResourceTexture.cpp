#include "ResourceTexture.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "DevIL/include/il.h"
#pragma comment (lib, "Devil/libx64/DevIL.lib")
#pragma comment (lib, "Devil/libx64/ILU.lib")
#pragma comment (lib, "Devil/libx64/ILUT.lib")

#include "mmgr/mmgr.h"

ResourceTexture::ResourceTexture()
{

}

ResourceTexture::~ResourceTexture()
{
}

void ResourceTexture::Import(const char* assetsPath, const char* libraryPath) //***
{
	// Load Texture from Assets Folder
	Texture texture = CreateTexture(assetsPath);

	// SaveOwnFormat
	SaveOwnFormat(texture, libraryPath);

	// Unload Texture
	FreeMemory(texture);
}

// --------------------
void ResourceTexture::LoadToMemory(Texture& texture, const char* path)
{
	texture = CreateTexture(path);
}

void ResourceTexture::FreeMemory(const Texture& texture) const
{
}

// --------------------
Texture& ResourceTexture::CreateTexture(const char* filepath)
{
	Texture texture;
	
	// Create Texture from file
	// ...

	return texture;
}

void ResourceTexture::SaveOwnFormat(const Texture& texture, const char* path)
{
	//// Save to DDS
	//// ...

	//char** buffer; //***
	//unsigned int size; //***

	//if (App->filesystem->Save(path, buffer, size) <= 0)
	//{
	//	LOG("Error saving texture with path: %s", path);
	//}
}

