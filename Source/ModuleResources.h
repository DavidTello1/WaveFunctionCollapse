#pragma once
#include "Module.h"
#include "Globals.h"

#include <vector>

class ResourceManager;

enum ResourceType {
	ANY = 0,
	TEXTURE,
	SHADER,
	SCENE
};

struct ResourceData {
	ResourceData() = default;
	ResourceData(UID id, ResourceType type, const char* assetsPath, const char* libaryPath, uint64_t lastModTime)
		: id(id), type(type), assetsPath(assetsPath), libraryPath(libraryPath), lastModTime(lastModTime) {}

	UID id = 0;
	ResourceType type = ANY;
	const char* assetsPath = "";
	const char* libraryPath = "";
	uint64_t lastModTime = 0;
};

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	virtual ~ModuleResources();

	bool Start() override;
	bool CleanUp() override;

	// ---
	UID ImportResource(const char* filepath); // Load resource from outside resource library & Create copy inside library (own-file-format)
	void SaveResource(UID id); // Save in resource library (own-file-format)
	void RemoveResource(UID id); // Remove resource from library & assets and delete metaFile

	void LoadResource(UID id); // Load from library
	void UnloadResource(UID id); // Unload from library

private:
	// Utils
	int FindResource(UID id) const;
	ResourceData* GetResourceData(UID id);
	const ResourceData* GetResourceData(UID id) const;
	const ResourceType GetResourceType(const char* filepath) const;
	const char* GetLibraryPath(UID id, ResourceType type) const;
	const char* NormalizePath(const char* path) const;

	// File Management
	UID ImportFromAssets(const char* assetsPath);
	//void ImportAllFiles(const char* folder, std::vector<const char*> exclude_extensions);
	//void HotReloading();

	// Meta Files
	const char* GetMetaPath(const char* filepath) const;
	bool ExistsMeta(const char* filepath) const;
	void RemoveMeta(const char* filepath);
	ResourceData LoadMeta(const char* filepath);
	void SaveMeta(const ResourceData& data);

private:
	std::vector<ResourceData> resourceDatas; // contains the ResourceData of all the resources inside Library Folder

	std::vector<ResourceManager*> resource_mgrs;
};
