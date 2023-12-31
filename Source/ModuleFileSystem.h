#pragma once
#include "Module.h"

#include <vector>
#include <string>

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(const char* game_path = nullptr);
	~ModuleFileSystem();

	bool Init() override;

	// ---
	unsigned int Load(const char* filepath, char** buffer) const;
	unsigned int Save(const char* filepath, const void* buffer, unsigned int size, bool append = false) const;

	bool Import(const char* filepath, const char* destination);
	bool Remove(const char* filepath);

	bool Copy(const char* source, const char* destination);
	bool Mount(const char* filepath);
	bool CreateFolder(const char* filepath);

	// Utility functions
	bool Exists(const char* filepath) const;
	bool IsFolder(const char* filepath) const;
	std::string NormalizePath(const char* filepath) const;

	// Getters
	std::string GetFileName(const char* filepath, bool extension = true) const;
	std::string GetExtension(const char* filepath) const;
	void GetFolderContent(const char* filepath, std::vector<std::string>& files, std::vector<std::string>& folders) const;
	const uint64_t GetLastModTime(const char* filepath) const;

	// Paths
	const char* GetBasePath() const;
	const char* GetWritePath() const;
	const char* GetReadPaths() const;
	const char* GetUserPath() const;
};
