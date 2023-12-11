#pragma once
#include "Resource.h"

struct Texture : public Resource {
	unsigned int index;
	double width;
	double height;
};

class ResourceTexture : public ResourceManager
{
public:
	ResourceTexture();
	~ResourceTexture();

	void Load(UID id, const char* path) override
	{
		Texture texture;
		int refCount = manager.LoadResource(id, texture);

		if (refCount == 1)
			LoadToMemory(texture, path);
	}

	void Unload(UID id) override
	{
		if (manager.UnloadResource(id))
			Remove(id);
	}

	void Remove(UID id) override
	{
		Texture texture;
		if (!manager.GetResource(id, texture))
			return;

		FreeMemory(texture);
		manager.RemoveResource(id);
	}

	void Save(UID id, const char* path) override
	{
		Texture texture;
		if (!manager.GetResource(id, texture))
			return;

		SaveOwnFormat(texture, path);
	}

	void Import(const char* assetsPath, const char* libraryPath) override;

private:
	void LoadToMemory(Texture& texture, const char* path);
	void FreeMemory(const Texture& texture) const;

	void SaveOwnFormat(const Texture& texture, const char* path);
	Texture& CreateTexture(const char* filepath);

private:
	ResourceContainer<Texture> manager;
};
