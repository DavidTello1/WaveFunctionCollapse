#include "ModuleResources.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "stb/stb_image.h"
#include "Glew/include/glew.h"

#include <windows.h>
#include <fstream>

#include "mmgr/mmgr.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Start()
{
    // Default Shader
    defaultShader = shaders[LoadShader("Assets/Shaders/shaders.glsl", "DEFAULT_SHADER")]->index;

    // Default Texture (white 1x1)
    glGenTextures(1, &defaultTexture);
    glBindTexture(GL_TEXTURE_2D, defaultTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    uint32_t color = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

    return true;;
}

bool ModuleResources::Update(float dt)
{
    // Hot reloading
    for (unsigned int i = 0; i < shaders.size(); ++i)
    {
        Shader* shader = shaders[i];
        u64 currentTimeStamp = ModuleResources::GetFileLastWriteTimestamp(shader->filepath.c_str());

        if (currentTimeStamp > shader->timestamp)
        {
            glDeleteProgram(shader->index);
            std::string programSource = ModuleResources::ReadTextFile(shader->filepath.c_str());

            shader->index = ModuleResources::CreateShader(programSource, shader->name.c_str());
            shader->timestamp = currentTimeStamp;

            LOG("Successfully reloaded shader '{0}'", shader->name);
        }
    }
    return true;;
}

bool ModuleResources::CleanUp()
{
    // Textures
    for (unsigned int i = 0; i < textures.size(); ++i)
    {
        glDeleteTextures(1, &textures[i]->index);
        delete(textures[i]);
    }
    textures.clear();

    // Shaders
    for (unsigned int i = 0; i < shaders.size(); ++i)
        delete(shaders[i]);
    shaders.clear();

    return true;
}

// --------------------------------------------
std::string ModuleResources::ReadTextFile(const char* filepath)
{
    std::ifstream ifs(filepath);

    if (ifs)
    {
        std::string fileText((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        return  fileText;
    }

    LOG("fopen() failed reading file {0}", filepath);
    return "ERROR";
}

u64 ModuleResources::GetFileLastWriteTimestamp(const char* filepath)
{
    union Filetime2u64 {
        FILETIME filetime;
        u64      u64time;
    } conversor;

    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (GetFileAttributesExA(filepath, GetFileExInfoStandard, &Data)) {
        conversor.filetime = Data.ftLastWriteTime;
        return(conversor.u64time);
    }
    return 0;
}

// Programs
GLuint ModuleResources::CreateShader(std::string source, const char* name)
{
    GLchar  infoLogBuffer[1024] = {};
    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
    GLsizei infoLogSize;
    GLint   success;

    char versionString[] = "#version 330\n";
    char shaderNameDefine[128];
    sprintf_s(shaderNameDefine, "#define %s\n", name);
    char vertexShaderDefine[] = "#define VERTEX\n";
    char fragmentShaderDefine[] = "#define FRAGMENT\n";

    const GLchar* vertexShaderSource[] = {
        versionString,
        shaderNameDefine,
        vertexShaderDefine,
        source.c_str()
    };
    const GLint vertexShaderLengths[] = {
        (GLint)strlen(versionString),
        (GLint)strlen(shaderNameDefine),
        (GLint)strlen(vertexShaderDefine),
        (GLint)source.length()
    };
    const GLchar* fragmentShaderSource[] = {
        versionString,
        shaderNameDefine,
        fragmentShaderDefine,
        source.c_str()
    };
    const GLint fragmentShaderLengths[] = {
        (GLint)strlen(versionString),
        (GLint)strlen(shaderNameDefine),
        (GLint)strlen(fragmentShaderDefine),
        (GLint)source.length()
    };

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        LOG("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", name, infoLogBuffer);
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        LOG("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", name, infoLogBuffer);
    }

    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vshader);
    glAttachShader(programHandle, fshader);
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        LOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", name, infoLogBuffer);
    }

    glUseProgram(0);

    glDetachShader(programHandle, vshader);
    glDetachShader(programHandle, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programHandle;
}

u64 ModuleResources::LoadShader(const char* filepath, const char* name)
{
    std::string source = ModuleResources::ReadTextFile(filepath);

    Shader* shader = new Shader();
    shader->index = CreateShader(source, name);
    shader->filepath = filepath;
    shader->name = name;
    shader->timestamp = ModuleResources::GetFileLastWriteTimestamp(filepath);

    // getting info from the shader
    GLint attributeSize = 0;
    GLenum attributeType = 0;
    const GLsizei bufferSize = 64;
    GLchar attributeName[bufferSize];

    GLint attributeCount = 0;
    glGetProgramiv(shader->index, GL_ACTIVE_ATTRIBUTES, &attributeCount);

    for (int i = 0; i < attributeCount; ++i)
    {
        VertexShaderAttribute attribute = {};
        GLsizei attributeNameLength = 0;

        glGetActiveAttrib(shader->index, (GLuint)i, bufferSize,
            &attributeNameLength, &attributeSize, &attributeType, attributeName);

        attribute.ncomponents = attributeSize;
        attribute.location = glGetAttribLocation(shader->index, attributeName);

        shader->vertexShaderLayout.attributes.push_back(attribute);
    }

    shaders.push_back(shader);
    return shaders.size() - 1;
}

GLuint ModuleResources::CreateTexture(Image image)
{
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat = GL_RGB;
    GLenum dataType = GL_UNSIGNED_BYTE;

    switch (image.nchannels)
    {
    case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
    case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
    default: LOG("CreateTexture() - Unsupported number of channels");
    }

    GLuint texHandle;
    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size.x, image.size.y, 0, dataFormat, dataType, image.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texHandle;
}

Texture* ModuleResources::LoadTexture(const char* filepath)
{
    // Check if already loaded
    for (Texture* tex : textures)
    {
        if (tex->filepath == filepath)
            return tex;
    }

    Image image = {};
    stbi_set_flip_vertically_on_load(false);
    image.pixels = stbi_load(filepath, &image.size.x, &image.size.y, &image.nchannels, 0); // load image
    if (image.pixels)
    {
        image.stride = image.size.x * image.nchannels;

        Texture* tex = new Texture();
        tex->index = CreateTexture(image);
        tex->filepath = filepath;
        tex->size = image.size;

        textures.push_back(tex);

        stbi_image_free(image.pixels); // free image

        LOG("Texture loaded correctly: %s", filepath);
        return tex;
    }

    LOG("Texture could not be loaded: %s", filepath);
    return nullptr;
}

void ModuleResources::SaveJson(const char* filepath, json& data)
{
    if (filepath == nullptr)
    {
        LOG("Error saving json file %s", filepath);
        return;
    }

    std::string file = data.dump(4); // serialize json

    char* buffer = (char*)file.data();
    uint size = file.length();

    App->filesystem->Save(filepath, buffer, size);
}

json ModuleResources::LoadJson(const char* filepath)
{
    json file;
    if (filepath == nullptr)
    {
        LOG("Error creating json file %s", filepath);
        return file;
    }

    std::ifstream ifs;
    ifs.open(filepath); // open file

    if (!ifs.is_open())
    {
        LOG("Error unable to open json file %s", filepath);
        return file;
    }

    try
    {
        file = json::parse(ifs); // parse file
    }
    catch (json::parse_error& e)
    {
        LOG("Error parsing json file: %s", e.what());
    }

    ifs.close(); // close file

    return file;
}
