#include "ModuleRenderer.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleGUI.h"

#include "Scene.h"
#include "Camera.h"

#include "SDL/include/SDL_video.h"
#include "glm/include/glm/gtc/type_ptr.hpp"

#include "Glew/include/glew.h"
#pragma comment(lib, "glew/x64/glew32.lib")
#pragma comment (lib, "glu32.lib")    // link OpenGL Utility lib
#pragma comment (lib, "opengl32.lib") // link Microsoft OpenGL lib

#include "mmgr/mmgr.h"

ModuleRenderer::ModuleRenderer(bool start_enabled) : Module(start_enabled)
{
	context = NULL;
}

ModuleRenderer::~ModuleRenderer()
{
}

bool ModuleRenderer::Init()
{
	LOG("Creating Renderer context\n");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->GetWindow());
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	// Initialize glew
	GLenum error = glewInit();
	LOG("Loading glew");

	// OpenGL
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create Basic Quad
	CreateQuad();

	// --- Events
	App->event->Subscribe(this, &ModuleRenderer::OnResize);

	return true;
}

bool ModuleRenderer::PreUpdate(float dt)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

bool ModuleRenderer::PostUpdate(float dt)
{
	// Draw Scene
	App->scene->Draw();

	// Draw UI
	App->gui->Draw();

	SDL_GL_SwapWindow(App->window->GetWindow());
	return true;
}

// Called before quitting
bool ModuleRenderer::CleanUp()
{
	LOG("Renderer CleanUp");

	SDL_GL_DeleteContext(context);

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	return true;
}

// --------------------------------
void ModuleRenderer::UpdateViewportSize()
{
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
}

void ModuleRenderer::OnResize(EventWindowResize* e)
{
	glViewport(0, 0, e->width, e->height);
}

void ModuleRenderer::CreateQuad()
{
	float vertices[] = {
		// positions        // texture coords
		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,	1.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2, // first triangle
		0, 3, 1  // second triangle
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &quadIBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void ModuleRenderer::DrawQuadEx(const unsigned int shader, const glm::mat4 viewProjMatrix, const glm::mat4 modelMatrix, const uint32_t texture, const glm::vec4 color)
{
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uViewProj"), 1, GL_FALSE, (GLfloat*)&viewProjMatrix);

	glUniformMatrix4fv(glGetUniformLocation(shader, "uTransform"), 1, GL_FALSE, (GLfloat*)&modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);

	glUniform4f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b, color.a);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// -------------------------------------
void ModuleRenderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	// Get Camera
	Camera* camera = App->scene->GetCurrentScene()->GetCamera();
	if (camera == nullptr)
	{
		LOG("Unable to get camera")
			return;
	}

	// Get Model Matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	// Draw Quad
	DrawQuadEx(App->resources->GetDefaultShader(), camera->GetViewProjMatrix(), model, App->resources->GetDefaultTexture(), color);
}

void ModuleRenderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const uint32_t texture)
{
	// Get Camera
	Camera* camera = App->scene->GetCurrentScene()->GetCamera();
	if (camera == nullptr)
	{
		LOG("Unable to get camera")
			return;
	}

	// Get Model Matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	// Draw Quad
	DrawQuadEx(App->resources->GetDefaultShader(), camera->GetViewProjMatrix(), model, texture, glm::vec4(1.0f));

}

void ModuleRenderer::DrawQuadRotated(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const float& rotation, const glm::vec2& center)
{
	// Get Camera
	Camera* camera = App->scene->GetCurrentScene()->GetCamera();
	if (camera == nullptr)
	{
		LOG("Unable to get camera")
			return;
	}

	// Get Model Matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	if (center != glm::vec2(0.0f))
	{
		model = glm::translate(model, glm::vec3(center, 0.0f));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-center, 0.0f));
	}
	else
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	// Draw Quad
	DrawQuadEx(App->resources->GetDefaultShader(), camera->GetViewProjMatrix(), model, App->resources->GetDefaultTexture(), color);
}

void ModuleRenderer::DrawQuadRotated(const glm::vec2& position, const glm::vec2& size, const uint32_t texture, const float& rotation, const glm::vec2& center)
{
	// Get Camera
	Camera* camera = App->scene->GetCurrentScene()->GetCamera();
	if (camera == nullptr)
	{
		LOG("Unable to get camera")
			return;
	}

	// Get Model Matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	if (center != glm::vec2(0.0f))
	{
		model = glm::translate(model, glm::vec3(center, 0.0f));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-center, 0.0f));
	}
	else
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	// Draw Quad
	DrawQuadEx(App->resources->GetDefaultShader(), camera->GetViewProjMatrix(), model, texture, glm::vec4(1.0f));
}