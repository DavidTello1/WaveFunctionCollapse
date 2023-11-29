#pragma once
#include "UI_Element.h"
#include "Color.h"

#include "DynArray.h"
#include "List.h"

class UI_Button;

class ButtonGrid : public UI_Element
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
	void Update(float dt) override;
	void Draw() override;

	// --- Getters
	const unsigned int GetSpacing() const { return spacing; }
	const Type GetSelectionType() const { return type; }
	const List<unsigned int>& GetSelected() const { return selected; }

	// --- Setters
	void SetPos(int x, int y) { this->x = x; this->y = y; UpdatePositions(); }
	void SetColumns(int columns) { Resize(columns, rows); }
	void SetRows(int rows) { Resize(columns, rows); }
	void SetSpacing(unsigned int spacing) { this->spacing = spacing; UpdatePositions(); }
	void SetSelectionType(Type type) { this->type = type; }

	// --- Utils
	void Resize(const int columns, const int rows);
	void UnSelectAll();

private:
	void UpdatePositions();

private:
	int columns, rows;
	unsigned int buttonSize;
	unsigned int spacing;
	Type type;

	Color idleColor, hoverColor, selectedColor;

	DynArray<UI_Button*> buttons;
	List<unsigned int> selected;
};
