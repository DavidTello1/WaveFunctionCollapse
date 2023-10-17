#include "UI_Button.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer.h"
#include "ModuleScene.h"
#include "Scene.h"
#include "Camera.h"

#include "SDL/include/SDL_mouse.h"
#include "SDL/include/SDL_scancode.h"

void UI_Button::Update()
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

	int posX = x;
	int posY = y;
	if (isStatic)
	{
		int cameraX = App->scene->GetCurrentScene()->GetCamera()->GetPosition().x;
		int cameraY = App->scene->GetCurrentScene()->GetCamera()->GetPosition().y;

		posX += cameraX;
		posY += cameraY;
	}

	App->renderer->DrawQuad(glm::vec2(posX, posY), glm::vec2(width, height), glm::vec4(color.r, color.g, color.b, color.a));
}

bool UI_Button::IsHovered() const
{
	int cameraX = App->scene->GetCurrentScene()->GetCamera()->GetPosition().x;
	int cameraY = App->scene->GetCurrentScene()->GetCamera()->GetPosition().y;

	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);

	if (!isStatic)
	{
		mouseX += cameraX;
		mouseY += cameraY;
	}

	return (mouseX >= x && mouseX < x + width && mouseY >= y && mouseY < y + height);
}
