#pragma once
#include "Color.h"

#include "DynArray.h"
#include "List.h"

class UI_Button;

class ButtonGrid
{
public:
	enum Type {
		SINGLE_SELECTION = 0,
		MULTIPLE_SELECTION
	};

public:
	ButtonGrid(int x, int y, int width, int height, unsigned int buttonSize, unsigned int spacing = 0, Type type = SINGLE_SELECTION);
	~ButtonGrid();

	// --- Logic
	void Update();
	void Draw();

	// --- Getters
	const int GetPosX() const { return x; }
	const int GetPosY() const { return y; }
	const int GetWidth() const { return width; }
	const int GetHeight() const { return height; }
	const unsigned int GetSpacing() const { return spacing; }
	const Type GetSelectionType() const { return type; }
	const List<unsigned int>& GetSelected() const { return selected; }

	// --- Setters
	void SetPosition(int x, int y) { this->x = x; this->y = y; UpdatePositions(); }
	void SetWidth(int width) { Resize(width, height);}
	void SetHeight(int height) { Resize(width, height);}
	void SetSpacing(unsigned int spacing) { this->spacing = spacing; UpdatePositions();}
	void SetSelectionType(Type type) { this->type = type; }

	// --- Utils
	void Resize(const int width, const int height);
	void UnSelectAll();

private:
	void UpdatePositions();

private:
	int x, y;
	int width, height;
	unsigned int buttonSize;
	unsigned int spacing;
	Type type;

	Color idleColor, hoverColor, selectedColor;

	DynArray<UI_Button*> buttons;
	List<unsigned int> selected;
};
