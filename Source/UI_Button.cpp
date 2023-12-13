#include "UI_Button.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer.h"
#include "ModuleScene.h"
#include "Scene.h"
#include "Camera.h"

#include "SDL/include/SDL_mouse.h"
#include "SDL/include/SDL_scancode.h"
#include "glm/include/glm/gtc/type_ptr.hpp"

void UI_Button::Update(float dt)
{
	isClicked = false;

	if (!IsHovered())
		return;

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		isClicked = true;
		isSelected = !isSelected;
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && 
		(App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT))
	{
		isClicked = true;
		isSelected = true;
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT &&
		(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_REPEAT))
	{
		isClicked = true;
		isSelected = false;
	}
}

void UI_Button::Draw()
{
	Color color = idleColor;
	if (isSelected)
		color = selectedColor;
	else if (IsHovered())
		color = hoverColor;

	Camera* camera = App->scene->GetCurrentScene()->GetCamera();
	if (camera == nullptr)
		return;

	float zoom = camera->GetZoom();

	glm::vec4 rect = glm::vec4(x, y, width, height);

	if (isStatic)
	{
		rect.x += camera->GetPosition().x;
		rect.y += camera->GetPosition().y;
		rect.z *= camera->GetZoom();
		rect.w *= camera->GetZoom();
	}

	App->renderer->DrawQuad(glm::vec2(rect.x, rect.y), glm::vec2(rect.z, rect.w), glm::vec4(color.r, color.g, color.b, color.a));
}
