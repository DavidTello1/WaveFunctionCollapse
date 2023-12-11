#include "ResourceShader.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "Glew/include/glew.h"

#include <string>
#include <fstream>

#include "mmgr/mmgr.h"

void ResourceShader::Import(const char* assetsPath, const char* libraryPath)
{
    //*** consider saving to a different format like unity or unreal do

    // We are only copying the file to Library Folder
    if (!App->filesystem->Copy(assetsPath, libraryPath))
    {
        LOG("Error importing shader from %s to %s", assetsPath, libraryPath);
    }
}

// --------------------
void ResourceShader::LoadToMemory(Shader& shader, const char* path)
{
    // Load Shader File
    char* source = nullptr;
    if (App->filesystem->Load(path, &source) == 0)
    {
        LOG("Error loading shader from %s", path);
    }

    // Create Shader from file
    shader.index = CreateShader(source);

    // Fill Shader Layout
    GLint attributeSize = 0;
    GLenum attributeType = 0;
    const GLsizei bufferSize = 64;
    GLchar attributeName[bufferSize];

    GLint attributeCount = 0;
    glGetProgramiv(shader.index, GL_ACTIVE_ATTRIBUTES, &attributeCount);

    for (int i = 0; i < attributeCount; ++i)
    {
        VertexShaderAttribute attribute = {};
        GLsizei attributeNameLength = 0;

        glGetActiveAttrib(shader.index, (GLuint)i, bufferSize,
            &attributeNameLength, &attributeSize, &attributeType, attributeName);

        attribute.ncomponents = attributeSize;
        attribute.location = glGetAttribLocation(shader.index, attributeName);

        shader.layout.attributes.push_back(attribute);
    }
}

void ResourceShader::FreeMemory(const Shader& shader) const
{
    if (glIsProgram(shader.index))
    {
        glDeleteProgram(shader.index);
    }
}

// --------------------
unsigned int ResourceShader::CreateShader(const char* filepath)
{
    Shader shader;

    GLchar  infoLogBuffer[1024] = {};
    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
    GLsizei infoLogSize;
    GLint   success;

    char versionString[] = "#version 330\n";
    char vertexShaderDefine[] = "#define VERTEX\n";
    char fragmentShaderDefine[] = "#define FRAGMENT\n";

    const GLchar* vertexShaderSource[] = {
        versionString,
        vertexShaderDefine,
        filepath
    };
    const GLint vertexShaderLengths[] = {
        (GLint)strlen(versionString),
        (GLint)strlen(vertexShaderDefine),
        (GLint)strlen(filepath)
    };
    const GLchar* fragmentShaderSource[] = {
        versionString,
        fragmentShaderDefine,
        filepath
    };
    const GLint fragmentShaderLengths[] = {
        (GLint)strlen(versionString),
        (GLint)strlen(fragmentShaderDefine),
        (GLint)strlen(filepath)
    };

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        LOG("glCompileShader() failed with vertex shader: %s\nReported message:\n%s\n", filepath, infoLogBuffer);
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        LOG("glCompileShader() failed with fragment shader: %s\nReported message:\n%s\n", filepath, infoLogBuffer);
    }

    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vshader);
    glAttachShader(programHandle, fshader);
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        LOG("glLinkProgram() failed with program: %s\nReported message:\n%s\n", filepath, infoLogBuffer);
    }

    glUseProgram(0);

    glDetachShader(programHandle, vshader);
    glDetachShader(programHandle, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programHandle;
}

void ResourceShader::SaveOwnFormat(const Shader& shader, const char* path)
{
   // We don't have an own file format for shaders so we do nothing
}
