#include "UI_Button.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer.h"

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

	App->renderer->DrawQuad(glm::vec2(x, y), glm::vec2(width, height), glm::vec4(color.r, color.g, color.b, color.a));
}

bool UI_Button::IsHovered() const
{
	int mouseX = 0;
	int mouseY = 0;
	App->input->GetMousePosition(mouseX, mouseY);

	return (mouseX >= x && mouseX < x + width && mouseY >= y && mouseY < y + height);
}
