#pragma once

class UI_Element
{
public:
	// --- Constructor
	UI_Element() : x(0), y(0), width(0), height(0), layer(0) {};
	UI_Element(int x, int y, int width, int height, int layer = 0) : x(x), y(y), width(width), height(height), layer(layer) {};

	// --- Destructor
	virtual ~UI_Element() {};

	// --- Main functions
	virtual void Update(float dt) {};
	virtual void Draw() {};

	// --- Getters
	int GetPosX() const { return x; }
	int GetPosY() const { return y; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetLayer() const { return layer; }

	// --- Setters
	void SetPosX(int x) { this->x = x; }
	void SetPosY(int y) { this->y = y; }
	void SetPos(int x, int y) { this->x = x; this->y = y; }
	void SetWidth(int width) { this->width = width; }
	void SetHeight(int height) { this->height = height; }
	void SetSize(int width, int height) { this->width = width; this->height = height; }
	void SetLayer(int layer) { this->layer = layer; }

protected:
	int x, y;
	int width, height;
	int layer;
};