#pragma once
#include "Color.h"

class UI_Button
{
	friend class ButtonGrid;

public:
	UI_Button(int x, int y, int width, int height, Color idleColor, Color hoverColor, Color selectedColor) {
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->idleColor = idleColor;
		this->hoverColor = hoverColor;
		this->selectedColor = selectedColor;
		this->isSelected = false;
	}
	~UI_Button() {}

	// --- Logic
	void Update();
	void Draw();

	// --- Utils
	bool IsHovered() const;
	bool IsClicked() const { return isClicked; }
	bool IsSelected() const { return isSelected; }

	bool IsStatic() const { return isStatic; }
	void SetStatic(bool value) { isStatic = value; }

	const int GetPosX() const { return x; }
	const int GetPosY() const { return y; }
	const int GetWidth() const { return width; }
	const int GetHeight() const { return height; }

	void SetPos(int x, int y) { this->x = x; this->y = y; }
	void SetWidth(int width) { this->width = width; }
	void SetHeight(int height) { this->height = height; }
	void Resize(int width, int height) { this->width = width; this->height = height; }

private:
	int x, y;
	int width, height;

	bool isStatic = false;
	bool isClicked = false;
	bool isSelected = false;

	Color idleColor, hoverColor, selectedColor;
};