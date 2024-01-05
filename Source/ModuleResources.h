#pragma once
#include "Module.h"

#include "json/json.hpp"
#include "glm/include/glm/glm.hpp"

#include <vector>
#include <string>

typedef unsigned int GLuint;
typedef unsigned long long u64;
typedef nlohmann::json json;

// --- Shader ---
struct VertexShaderAttribute
{
	unsigned char location;
	unsigned char ncomponents;
};

struct VertexShaderLayout
{
	std::vector<VertexShaderAttribute> attributes;
};

struct Shader
{
	GLuint             index;
	std::string        filepath;
	std::string        name;
	u64                timestamp;
	VertexShaderLayout vertexShaderLayout;
};

// --- Texture ---
struct Image
{
	void* pixels;
	glm::ivec2	size;
	int			nchannels;
	int			stride;
};

struct Texture
{
	GLuint      index;
	std::string filepath;
	glm::ivec2	size;
};

// -------------------------------------------
class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	virtual ~ModuleResources();

	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	// --- Utils
	const GLuint GetDefaultShader() const { return defaultShader; }
	const GLuint GetDefaultTexture() const { return defaultTexture; }

	static std::string ReadTextFile(const char* filepath); // Reads a whole file and returns a string with its contents. Returned string is temporary.
	static u64 GetFileLastWriteTimestamp(const char* filepath); // Retrieves a timestamp indicating the last time the file was modified. Useful for hot reloads.
	std::string NormalizePath(const char* filepath) const;
	std::string GetFileName(const char* filepath, bool extension = true) const;
	std::string GetExtension(const char* filepath) const;

	// --- Shaders
	static GLuint CreateShader(std::string filepath, const char* name);
	u64 LoadShader(const char* filepath, const char* name);

	// --- Textures
	static GLuint CreateTexture(Image image);
	Texture* LoadTexture(const char* filepath);

	// --- Jsons
	void SaveJson(const char* filepath, json& data);
	json LoadJson(const char* filepath);

public:
	std::vector<Texture*> textures;
	std::vector<Shader*> shaders;

	GLuint defaultShader = 0;
	GLuint defaultTexture = 0;
};
