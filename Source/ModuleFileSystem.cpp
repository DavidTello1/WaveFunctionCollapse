#include "ModuleFileSystem.h"
#include "Globals.h"

#include "SDL/include/SDL_filesystem.h"

#include "PhysFS/include/physfs.h"
#pragma comment(lib, "PhysFS/libx64/physfs.lib")

#include "mmgr/mmgr.h"

ModuleFileSystem::ModuleFileSystem(const char* game_path)
{
	// needs to be created before Init so other modules can use it
	LOG("Loading PhysFS");
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	// workaround VS string directory mess
	Mount(".");
	if (0 && game_path != nullptr)
		Mount(game_path);

	LOG("FileSystem Operations base is [%s] plus:", GetBasePath(), 'd');
	LOG(GetReadPaths());

	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError(), 'e');

	// Create Standard Paths
	const char* dirs[] = {
		FOLDER_SETTINGS,
		FOLDER_ASSETS,
		FOLDER_LIBRARY,

		FOLDER_LIBRARY_SCENES,
		FOLDER_LIBRARY_TEXTURES,
		FOLDER_LIBRARY_SHADERS
	};

	for (unsigned int i = 0; i < sizeof(dirs) / sizeof(const char*); ++i)
	{
		if (!Exists(dirs[i]))
			CreateFolder(dirs[i]);
	}
}

ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

bool ModuleFileSystem::Init()
{
	LOG("Loading File System");
	bool ret = true;

	// Ask SDL for a write dir
	char* write_path = SDL_GetPrefPath(ORGANIZATION, TITLE);

	//// Turn this on while in game mode
	//if(PHYSFS_setWriteDir(write_path) == 0)
		//LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	SDL_free(write_path);

	return ret;
}

// --------------------------------------------------------
// Opens a file to read and returns its size, 0 if error has occurred
// Data is loaded from filepath into buffer
unsigned int ModuleFileSystem::Load(const char* filepath, char** buffer) const
{
	PHYSFS_file* fs_file = PHYSFS_openRead(filepath);

	if (fs_file == nullptr)
	{
		LOG("Error opening file %s: %s", filepath, PHYSFS_getLastError());
		return 0;
	}

	PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);

	unsigned int ret = 0;
	if (size > 0)
	{
		*buffer = new char[size];
		unsigned int read = (unsigned int)PHYSFS_read(fs_file, *buffer, 1, size);

		if (read != size)
		{
			LOG("Error reading from file %s: %s", filepath, PHYSFS_getLastError());
			RELEASE(buffer);
		}
		else
			ret = read;
	}

	if (PHYSFS_close(fs_file) == 0)
		LOG("Error closing file %s: %s", filepath, PHYSFS_getLastError());

	return ret;
}

// Saves a whole buffer to disk, returns size of data saved or 0 if error has occurred
// Data from buffer with specified size is saved to filepath
// Set append to true if you want to append data or set it to false if you want to overwrite file
unsigned int ModuleFileSystem::Save(const char* filepath, const void* buffer, unsigned int size, bool append) const
{
	bool overwrite = PHYSFS_exists(filepath) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(filepath) : PHYSFS_openWrite(filepath);

	if (fs_file == nullptr)
	{
		LOG("Error opening file %s: %s", filepath, PHYSFS_getLastError());
		return 0;
	}

	unsigned int written = (unsigned int)PHYSFS_write(fs_file, (const void*)buffer, 1, size);

	unsigned int ret = 0;
	if (written != size)
	{
		LOG("Error writing to file %s: %s", filepath, PHYSFS_getLastError());
	}
	else
	{
		if (append == true)
		{
			LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), filepath);
		}
		else if (overwrite == false)
			LOG("New file created [%s%s] of %u bytes", PHYSFS_getWriteDir(), filepath, size);

		ret = written;
	}

	if (PHYSFS_close(fs_file) == 0)
		LOG("Error closing file %s: %s", filepath, PHYSFS_getLastError());

	return ret;
}

// Import a file from non-virtual filesystem, returns false on error
// Copies data from filepath to destination
bool ModuleFileSystem::Import(const char* filepath, const char* destination)
{
	FILE* source = nullptr;
	fopen_s(&source, filepath, "rb");
	PHYSFS_file* dest = PHYSFS_openWrite(destination);

	if (!source || !dest)
	{
		LOG("Error while importing from [%s] to [%s]", filepath, destination);
		return false;
	}

	static const int maxSize = 8192;
	char buf[maxSize];
	size_t size;

	while (size = fread_s(buf, maxSize, 1, maxSize, source))
		PHYSFS_write(dest, buf, 1, size);

	fclose(source);
	PHYSFS_close(dest);

	LOG("File System copied file [%s] to [%s]", filepath, destination);
	return true;
}

// Removes a file, returns false on error
bool ModuleFileSystem::Remove(const char* filepath)
{
	if (filepath == nullptr)
	{
		LOG("Error trying to delete [%s]: path not valid", filepath);
		return false;
	}

	if (PHYSFS_delete(filepath) != 0)
	{
		LOG("File deleted: [%s]", filepath);
		return true;
	}

	LOG("Error trying to delete [%s]: %s", filepath, PHYSFS_getLastError());
	return false;
}

// Copies a file from source to destination
// Returns false on error
bool ModuleFileSystem::Copy(const char* source, const char* destination)
{

	PHYSFS_file* src = PHYSFS_openRead(source);
	PHYSFS_file* dest = PHYSFS_openWrite(destination);

	PHYSFS_sint32 size;
	if (!src || !dest)
	{
		LOG("Error copying from [%s] to [%s]", source, destination);
		return false;
	}

	static const int maxSize = 8192;
	char buf[maxSize];

	while (size = (PHYSFS_sint32)PHYSFS_read(src, buf, 1, maxSize))
		PHYSFS_write(dest, buf, 1, size);

	PHYSFS_close(src);
	PHYSFS_close(dest);

	LOG("File System copied file [%s] to [%s]", source, destination);
	return true;
}

// Add a file or zip to the filesystem, returns false on error
bool ModuleFileSystem::Mount(const char* filepath)
{
	if (PHYSFS_mount(filepath, nullptr, 1) == 0)
	{
		LOG("Error Mounting file: %s", PHYSFS_getLastError());
		return false;
	}

	return true;
}

// Creates a new folder, returns false on error
bool ModuleFileSystem::CreateFolder(const char* filepath)
{
	if (PHYSFS_mkdir(filepath) == 0)
	{
		LOG("Error creating folder: %s", PHYSFS_getLastError());
		return false;
	}

	return true;
}

// Check if a file exists, returns false if not found
bool ModuleFileSystem::Exists(const char* filepath) const
{
	return (PHYSFS_exists(filepath) != 0);
}

// Check if a file is a folder, returns false if not
bool ModuleFileSystem::IsFolder(const char* filepath) const
{
	return (PHYSFS_isDirectory(filepath) != 0);
}

// Get the file name from path (baker_house.fbx)
const char* ModuleFileSystem::GetFileName(const char* filepath) const
{
	static const int maxSize = 128;

	const char* name = strrchr(filepath, maxSize);
	if (name == nullptr)
		name = (strrchr(filepath, '/') != nullptr) ? strrchr(filepath, '/') : "";

	if (name != "")
		name++;

	return name;
}

// Get the extension from path (fbx)
const char* ModuleFileSystem::GetExtension(const char* filepath) const
{
	static const int maxSize = 32;

	char buffer[maxSize] = "";
	const char* last_dot = strrchr(filepath, '.');
	if (last_dot != nullptr)
		strcpy_s(buffer, last_dot + 1);

	const char* extension = buffer;
	return extension;
}

// Get the files and folders inside a directory
// Fills the arrays in the parameters
void ModuleFileSystem::GetFolderContent(const char* filepath, std::vector<std::string>& files, std::vector<std::string>& folders) const
{
	char** rc = PHYSFS_enumerateFiles(filepath); 
	char** i;

	std::string dir(filepath);
	for (i = rc; *i != nullptr; i++)
	{
		std::string str = std::string(filepath) + std::string("/") + std::string(*i);
		if (IsFolder(str.c_str()))
			folders.push_back(*i);
		else
			files.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

// Get the last modification time of a file
// The modtime is returned as a number of seconds since the epoch (1 Jan 1970) or -1 on error
const uint64_t ModuleFileSystem::GetLastModTime(const char* filepath) const
{
	return PHYSFS_getLastModTime(filepath);
}

// Get the path where the application resides
// Returns a Read-Only string of base dir in platform-dependent notation
const char* ModuleFileSystem::GetBasePath() const
{
	return PHYSFS_getBaseDir();
}

// Get the path where PhysFS will allow file writing
// Returns a Read-Only string of write dir in platform-dependent notation or NULL if not set
const char* ModuleFileSystem::GetWritePath() const
{
	return PHYSFS_getWriteDir();
}

// Get the current search path
// Returns an empty list by default or an array of strings with a NULL entry to signify the end of the list
// Returns NULL on error
const char* ModuleFileSystem::GetReadPaths() const
{
	static const int maxSize = 512;

	static char paths[maxSize];
	paths[0] = '\0';

	char** path;
	for (path = PHYSFS_getSearchPath(); *path != nullptr; path++)
	{
		strcat_s(paths, maxSize, *path);
		strcat_s(paths, maxSize, "\n");
	}

	return paths;
}

// Get the path where the user's home directory resides
// Returns a Read-Only string of user dir in platform-dependent notation
const char* ModuleFileSystem::GetUserPath() const
{
	return PHYSFS_getUserDir();
}
