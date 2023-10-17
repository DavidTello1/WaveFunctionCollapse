#pragma once
#include "Module.h"

#include "glm/include/glm/glm.hpp"

struct EventWindowResize;

typedef void* SDL_GLContext;

class ModuleRenderer : public Module
{
public:
	ModuleRenderer(bool start_enabled = true);
	virtual ~ModuleRenderer();

	bool Init() override;
	bool PreUpdate(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	// --- Utils
	const SDL_GLContext& GetContext() const { return context; }
	void UpdateViewportSize();

	// --- Draw Functions
	void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
	void DrawQuad(const glm::vec2& position, const glm::vec2& size, const uint32_t texture);

	void DrawQuadRotated(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f), const float& rotation = 0.0f, const glm::vec2& center = glm::vec2(0.0f));
	void DrawQuadRotated(const glm::vec2& position, const glm::vec2& size, const uint32_t texture, const float& rotation = 0.0f, const glm::vec2& center = glm::vec2(0.0f));

private:
	void CreateQuad();
	void DrawQuadEx(const unsigned int shader, const glm::mat4 viewProjMatrix, const glm::mat4 modelMatrix, const uint32_t texture, const glm::vec4 color);

	// --- EVENTS ---
	void OnResize(EventWindowResize* e);

private:
	SDL_GLContext context;

	// Quad
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	unsigned int quadIBO = 0;
};