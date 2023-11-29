#pragma once
#include "UI_Element.h"

#include "Color.h"

class UI_Button : public UI_Element
{
	friend class ButtonGrid;

public:
	UI_Button(int x, int y, int width, int height, Color idleColor, Color hoverColor, Color selectedColor, bool isStatic = false) :
		UI_Element(x, y, width, height, isStatic), 
		idleColor(idleColor), hoverColor(hoverColor), selectedColor(selectedColor), isSelected(false) {}

	~UI_Button() {}

	// --- Logic
	void Update(float dt) override;
	void Draw() override;

	// --- Utils
	bool IsSelected() const { return isSelected; }

private:
	bool isSelected = false;

	Color idleColor, hoverColor, selectedColor;
};