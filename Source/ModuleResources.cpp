#include "ModuleResources.h"
#include "Globals.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ResourceScene.h"

#include "mmgr/mmgr.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Start()
{
    //// Default Shader
    //defaultShader = shaders[LoadShader("Assets/Shaders/shaders.glsl", "DEFAULT_SHADER")]->index;

    //// Default Texture (white 1x1)
    //glGenTextures(1, &defaultTexture);
    //glBindTexture(GL_TEXTURE_2D, defaultTexture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //uint32_t color = 0xffffffff;
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

    // --------------------
    resource_mgrs.push_back(new ResourceTexture());
    resource_mgrs.push_back(new ResourceShader());
    //resource_mgrs.push_back(new ResourceScene());

    return true;;
}

bool ModuleResources::CleanUp()
{
    for (size_t i = 0; i < resource_mgrs.size(); ++i)
    {
        delete resource_mgrs[i];
    }
    resource_mgrs.clear();

    return true;
}

UID ModuleResources::ImportResource(const char* filepath)
{
    // Get Assets Path
    std::string fileName = App->filesystem->GetFileName(NormalizePath(filepath));

    std::string assetsPath = FOLDER_ASSETS + fileName;

    // Copy Resource in Assets Folder
    App->filesystem->Import(filepath, assetsPath.c_str());

    // Import Resource from Assets Folder
    return ImportFromAssets(assetsPath.c_str());
}

void ModuleResources::RemoveResource(UID id)
{
    const ResourceData* data = GetResourceData(id);

    if (data == nullptr)
    {
        LOG("Error removing resource, id not found: %d", id);
        return;
    }

    // Remove resource from manager
    resource_mgrs[(int)data->type]->Remove(id);

    // Remove file from library
    App->filesystem->Remove(data->libraryPath);

    // Remove meta file from assets
    RemoveMeta(data->assetsPath);

    // Remove ResourceData from array
    int index = FindResource(id);
    resourceDatas.erase(resourceDatas.begin() + index);
}

void ModuleResources::SaveResource(UID id)
{
    ResourceData* data = GetResourceData(id);

    // Save Resource in Library Folder
    resource_mgrs[(int)data->type]->Save(id, data->libraryPath);

    // Update Last Modfication Time
    data->lastModTime = App->filesystem->GetLastModTime(data->assetsPath);
}

void ModuleResources::LoadResource(UID id)
{
    const ResourceData* data = GetResourceData(id);

    if (data == nullptr)
    {
        LOG("Error loading resource, id not found: %d", id);
        return;
    }

    resource_mgrs[(int)data->type]->Load(id, data->libraryPath);
}

void ModuleResources::UnloadResource(UID id)
{
    const ResourceData* data = GetResourceData(id);

    if (data == nullptr)
    {
        LOG("Error loading resource, id not found: %d", id);
        return;
    }

    resource_mgrs[(int)data->type]->Unload(id);
}

// --------------------
int ModuleResources::FindResource(UID id) const
{
    for (size_t i = 0; i < resourceDatas.size(); ++i)
    {
        if (resourceDatas[i].id == id)
            return i;
    }
    return -1;
}

ResourceData* ModuleResources::GetResourceData(UID id)
{
    int index = FindResource(id);

    if (index == -1)
        return nullptr;

    return &resourceDatas[index];
}

const ResourceData* ModuleResources::GetResourceData(UID id) const
{
    int index = FindResource(id);

    if (index == -1)
        return nullptr;

    return &resourceDatas[index];
}

const ResourceType ModuleResources::GetResourceType(const char* filepath) const
{
    std::string ext = App->filesystem->GetExtension(filepath);

    // Texture
    if (ext == "png" ||
        ext == "jpg" ||
        ext == "bmp" ||
        ext == "tga")
    {
        return ResourceType::TEXTURE;
    }

    // Shader
    if (ext == "glsl")
        return ResourceType::SHADER;

    // Scene
    else if (ext == "scene")
        return ResourceType::SCENE;

    // INVALID
    LOG("Error invalid extension: %s", ext.c_str());
    return ResourceType::ANY;
}

const char* ModuleResources::GetLibraryPath(UID id, ResourceType type) const
{
    std::string path;

    switch (type)
    {
    case ResourceType::TEXTURE:
        path = FOLDER_LIBRARY_TEXTURES + std::string("%d", id) + std::string(EXTENSION_TEXTURE);
        break;

    case ResourceType::SHADER:
        path = FOLDER_LIBRARY_SHADERS + std::string("%d", id) + std::string(EXTENSION_SHADER);
        break;

    case ResourceType::SCENE:
        path = FOLDER_LIBRARY_SCENES + std::string("%d", id) + std::string(EXTENSION_SCENE);
        break;

    default:
        LOG("Error ResourceType invalid");
        return nullptr;
    }

    return path.c_str();
}

const char* ModuleResources::NormalizePath(const char* filepath) const
{
    std::string path = filepath;

    for (std::string::iterator it = path.begin(); it != path.end(); ++it)
    {
        if (*it == '\\')
        {
            *it = '/';
            continue;
        }
        *it = tolower(*it);
    }

    return path.c_str();
}

// --------------------
UID ModuleResources::ImportFromAssets(const char* assetsPath)
{
    ResourceData data;

    if (ExistsMeta(assetsPath))
    {
        data = LoadMeta(assetsPath);
    }
    else
    {
        // Fill ResourceData
        data.id = App->GenerateUID();
        data.type = GetResourceType(assetsPath);
        data.assetsPath = assetsPath;
        data.libraryPath = GetLibraryPath(data.id, data.type);
        data.lastModTime = App->filesystem->GetLastModTime(assetsPath);

        // Save Meta File
        SaveMeta(data);
    }

    // Add ResourceData to array //*** check if already exists? -> overwrite
    resourceDatas.push_back(data); //*** persistence of data?

    // Create Resource in Library Folder
    resource_mgrs[(int)data.type]->Import(data.assetsPath, data.libraryPath);

    return data.id;
}

// --------------------
const char* ModuleResources::GetMetaPath(const char* filepath) const
{
    std::string metaPath = filepath + std::string(".meta");
    return metaPath.c_str();
}

bool ModuleResources::ExistsMeta(const char* filepath) const
{
    const char* metaPath = GetMetaPath(filepath);
    return App->filesystem->Exists(metaPath);
}

void ModuleResources::RemoveMeta(const char* filepath)
{
    const char* metaPath = GetMetaPath(filepath);
    App->filesystem->Remove(metaPath);
}

ResourceData ModuleResources::LoadMeta(const char* filepath) //***
{
    //const char* metaPath = GetMetaPath(filepath);

    //char* buffer = nullptr;
    //App->filesystem->Load(metaPath, &buffer);
    //
    //ResourceData data;
    //data.id          = buffer[0];
    //data.type        = buffer[0];
    //data.assetsPath  = buffer[0];
    //data.libraryPath = buffer[0];
    //data.lastModTime = buffer[0];

    //return data;

    return ResourceData();
}

void ModuleResources::SaveMeta(const ResourceData& data) //***
{
    //const char* metaPath = GetMetaPath(data.assetsPath);

    //char* buffer = nullptr;
    //unsigned int size = 0;

    //buffer += std::to_string(data.id);
    //buffer += data.type;
    //buffer += data.assetsPath;
    //buffer += data.libraryPath;
    //buffer += std::to_string(data.lastModTime);

    //App->filesystem->Save(metaPath, buffer, size);
}
