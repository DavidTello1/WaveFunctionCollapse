#pragma once

class UI_Element
{
public:
	// --- Constructor
	UI_Element() : x(0), y(0), width(0), height(0), isStatic(false), isClicked(false) {};
	UI_Element(int x, int y, int width, int height, bool isStatic = false) : 
		x(x), y(y), width(width), height(height), isStatic(isStatic), isClicked(false) {};

	// --- Destructor
	virtual ~UI_Element() {};

	// --- Main functions
	virtual void Update(float dt) {};
	virtual void Draw() {};

	// --- Interactable
	bool IsHovered() const;
	bool IsClicked() const { return isClicked; }

	// --- Getters
	bool IsStatic() const { return isStatic; }
	int GetPosX() const { return x; }
	int GetPosY() const { return y; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	// --- Setters
	void SetStatic(bool value) { isStatic = value; }
	void SetPosX(int x) { this->x = x; }
	void SetPosY(int y) { this->y = y; }
	void SetPos(int x, int y) { this->x = x; this->y = y; }
	void SetWidth(int width) { this->width = width; }
	void SetHeight(int height) { this->height = height; }
	void SetSize(int width, int height) { this->width = width; this->height = height; }

protected:
	int x, y;
	int width, height;

	bool isStatic;
	bool isClicked;
};