#include "UI_Button.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer.h"

#include "SDL/include/SDL_mouse.h"
#include "SDL/include/SDL_scancode.h"

void UI_Button::Update()
{
	if (!IsHovered())
		return;

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		isSelected = !isSelected;
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && 
		(App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT))
	{
		isSelected = true;
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT &&
		(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_REPEAT))
	{
		isSelected = false;
	}
}

void UI_Button::Draw()
{
	glm::vec4 color = idleColor;
	if (isSelected)
		color = selectedColor;
	else if (IsHovered())
		color = hoverColor;

	App->renderer->DrawQuad(glm::vec2(x, y), glm::vec2(width, height), color);
}

bool UI_Button::IsHovered() const
{
	int mouseX = 0;
	int mouseY = 0;
	App->input->GetMousePosition(mouseX, mouseY);

	return (mouseX >= x && mouseX < x + width && mouseY >= y && mouseY < y + height);
}
