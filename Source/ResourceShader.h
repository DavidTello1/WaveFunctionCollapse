#pragma once
#include "Resource.h"

struct VertexShaderAttribute {
	unsigned char location;
	unsigned char ncomponents;
};

struct VertexShaderLayout {
	std::vector<VertexShaderAttribute> attributes;
};

struct Shader : public Resource {
	unsigned int index;
	VertexShaderLayout layout;
};

class ResourceShader : public ResourceManager
{
public:
	ResourceShader() {}
	~ResourceShader() {}

	void Load(UID id, const char* path) override
	{
		Shader shader;
		int refCount = manager.LoadResource(id, shader);

		if (refCount == 1)
			LoadToMemory(shader, path);
	}

	void Unload(UID id) override
	{
		if (manager.UnloadResource(id))
			Remove(id);
	}

	void Remove(UID id) override
	{
		Shader shader;
		if (!manager.GetResource(id, shader))
			return;

		FreeMemory(shader);
		manager.RemoveResource(id);
	}

	void Save(UID id, const char* path) override
	{
		Shader shader;
		if (!manager.GetResource(id, shader))
			return;

		SaveOwnFormat(shader, path);
	}

	void Import(const char* assetsPath, const char* libraryPath) override;

private:
	void LoadToMemory(Shader& shader, const char* path);
	void FreeMemory(const Shader& shader) const;

	void SaveOwnFormat(const Shader& shader, const char* path);
	unsigned int CreateShader(const char* filepath);

private:
	ResourceContainer<Shader> manager;
};
