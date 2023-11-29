#include "ButtonGrid.h"
#include "UI_Button.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene.h"
#include "Camera.h"

#include "SDL/include/SDL_mouse.h"
#include "SDL/include/SDL_scancode.h"

ButtonGrid::ButtonGrid(int x, int y, int columns, int rows, unsigned int buttonSize, unsigned int spacing, Type type) : 
	UI_Element(x, y, columns * (buttonSize + spacing), rows * (buttonSize + spacing)), 
	columns(columns), rows(rows), buttonSize(buttonSize), spacing(spacing), type(type)
{
	// Colors
	idleColor	  = { 0, 0, 0, 0	 };	// black, alpha 0
	hoverColor	  = { 0, 0, 1, 0.5f  };	// blue, alpha 0.5
	selectedColor = { 0, 0, 1, 0.75f }; // blue, alpha 0.75

	// Init Buttons Array
	for (int i = 0; i < columns; ++i)
	{
		for (int j = 0; j < rows; ++j)
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

void ButtonGrid::Update(float dt)
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

			selected.append(i);
		}
		else if (type == MULTIPLE_SELECTION && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT &&
			(App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT))
		{
			if (!button->isSelected)
			{
				button->isSelected = true;
				selected.append(i);
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
	for (int i = 0; i < columns; ++i)
	{
		for (int j = 0; j < rows; ++j)
		{
			int index = i + columns * j;

			buttons[index]->x = x + i * (buttonSize + spacing);
			buttons[index]->y = y + j * (buttonSize + spacing);
		}
	}

	width = columns * (buttonSize + spacing);
	height = rows * (buttonSize + spacing);
}

void ButtonGrid::Resize(const int columns, const int rows)
{
	this->columns = columns;
	this->rows = rows;

	// Delete buttons
	for (unsigned int i = 0; i < buttons.size(); ++i)
	{
		delete buttons[i];
	}
	buttons.clear();
	selected.clear();

	// Create buttons array with new size
	for (int i = 0; i < columns * rows; ++i)
	{
		buttons.push_back(new UI_Button(0, 0, buttonSize, buttonSize, idleColor, hoverColor, selectedColor));
	}
	UpdatePositions();
}

void ButtonGrid::UnSelectAll()
{
	for (unsigned int i = 0; i < selected.size(); ++i)
	{
		int index = selected.at(i);
		buttons[index]->isSelected = false;
	}
	selected.clear();
}