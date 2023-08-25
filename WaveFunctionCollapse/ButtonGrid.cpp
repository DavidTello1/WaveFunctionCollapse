#include "ButtonGrid.h"
#include "UI_Button.h"

#include "Application.h"
#include "ModuleInput.h"

#include "SDL/include/SDL_mouse.h"
#include "SDL/include/SDL_scancode.h"

ButtonGrid::ButtonGrid(int x, int y, int width, int height, unsigned int buttonSize, unsigned int spacing, Type type)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->buttonSize = buttonSize;
	this->spacing = spacing;
	this->type = type;

	// Colors
	idleColor	  = { 0, 0, 0, 0 };		// black, alpha 0
	hoverColor	  = { 0, 0, 1, 0.5f };	// blue, alpha 0.5
	selectedColor = { 0, 0, 1, 0.75f }; // blue, alpha 0.75

	// Init Buttons Array
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			int buttonX = x + i * (buttonSize + spacing);
			int buttonY = y + j * (buttonSize + spacing);
			buttons.push_back(new UI_Button(buttonX, buttonY, buttonSize, buttonSize, idleColor, hoverColor, selectedColor));
		}
	}

	selected = List<unsigned int>();
}

ButtonGrid::~ButtonGrid()
{
	for (unsigned int i = 0; i < buttons.size(); ++i)
	{
		delete buttons[i];
	}
	buttons.clear();
}

void ButtonGrid::Update()
{
	for (unsigned int i = 0; i < buttons.size(); ++i)
	{
		UI_Button* button = buttons[i];

		if (!button->IsHovered())
			continue;

		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			button->isSelected = !button->isSelected;

			if (!button->isSelected)
			{
				selected.erase(i);
				return;
			}

			if (type == SINGLE_SELECTION)
				UnSelectAll();

			selected.add(i);
		}
		else if (type == MULTIPLE_SELECTION && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT &&
			(App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT))
		{
			if (!button->isSelected)
			{
				button->isSelected = true;
				selected.add(i);
			}
		}
		else if (type == MULTIPLE_SELECTION && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT &&
			(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_REPEAT))
		{
			if (button->isSelected)
			{
				button->isSelected = false;
				selected.erase(i);
			}
		}

		return; // Only one button can be hovered at a time, if we found it we can return directly
	}
}

void ButtonGrid::Draw()
{
	for (unsigned int i = 0; i < buttons.size(); ++i)
	{
		buttons[i]->Draw();
	}
}

void ButtonGrid::UpdatePositions()
{
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			int index = i + width * j;

			buttons[index]->x = x + i * (buttonSize + spacing);
			buttons[index]->y = y + j * (buttonSize + spacing);
		}
	}

}

void ButtonGrid::Resize(const int width, const int height)
{
	this->width = width;
	this->height = height;

	// Delete buttons
	for (unsigned int i = 0; i < buttons.size(); ++i)
	{
		delete buttons[i];
	}
	buttons.clear();

	// Create buttons array with new size
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			int buttonX = x + i * (buttonSize + spacing);
			int buttonY = y + j * (buttonSize + spacing);
			buttons.push_back(new UI_Button(buttonX, buttonY, buttonSize, buttonSize, idleColor, hoverColor, selectedColor));
		}
	}

	// Update positions
	UpdatePositions();
}

void ButtonGrid::UnSelectAll()
{
	for (unsigned int i = 0; i < selected.size(); ++i)
	{
		int index = selected[i]->data;
		buttons[index]->isSelected = false;
	}
	selected.clear();
}