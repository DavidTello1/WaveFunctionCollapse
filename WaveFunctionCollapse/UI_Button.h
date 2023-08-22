#pragma once

#include "glm/include/glm/glm.hpp"

class UI_Button
{
	friend class ButtonGrid;

public:
	UI_Button(int x, int y, int width, int height, glm::vec4 idleColor, glm::vec4 hoverColor, glm::vec4 selectedColor) {
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
	bool IsSelected() const { return isSelected; }

	const int GetPosX() const { return x; }
	const int GetPosY() const { return y; }
	void SetPos(int x, int y) { this->x = x; this->y = y; }

private:
	int x, y;
	int width, height;
	bool isSelected = false;

	glm::vec4 idleColor, hoverColor, selectedColor;
};