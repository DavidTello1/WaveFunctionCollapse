//#pragma once
//
//struct SDL_Color;
//
//class UI_Button
//{
//	UI_Button(int x, int y, int width, int height, SDL_Color idleColor, SDL_Color hoverColor, SDL_Color selectedColor) {
//		this->x = x;
//		this->y = y;
//		this->width = width;
//		this->height = height;
//		this->idleColor = idleColor;
//		this->hoverColor = hoverColor;
//		this->selectedColor = selectedColor;
//		this->isSelected = false;
//	}
//
//	~UI_Button() {
//	}
//
//	void Update() {
//		if (!IsHovered())
//			return;
//
//		if (mouseButton == KEY_REPEAT)
//		{ 
//			isSelected = true; 
//		}
//		else if (mouseButton == KEY_DOWN)
//		{ 
//			isSelected = !isSelected; 
//		}
//	}
//
//	void Draw() {
//		SDL_Color color = idleColor;
//		if (isSelected)
//			color = selectedColor;
//		else if (IsHovered())
//			color = hoverColor;
//
//		DrawQuad(x, y, width, height, color);
//	}
//
//	bool IsHovered() const { return (mouseX >= x && mouseX < width && mouseY >= y && mouseY < height); }
//	bool IsSelected() const { return isSelected; }
//
//private:
//	int x, y;
//	int width, height;
//	bool isSelected = false;
//
//	SDL_Color idleColor, hoverColor, selectedColor;
//};