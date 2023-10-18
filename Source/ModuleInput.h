#pragma once
#include "Module.h"

#define NUM_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:

	ModuleInput(bool start_enabled = true);
	virtual ~ModuleInput();

	bool Init() override;
	bool Start() override;
	bool PreUpdate(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	KEY_STATE GetKey(int id) const { return keyboard[id]; }
	KEY_STATE GetMouseButton(int id) const { return mouseButtons[id - 1]; }

	// Get mouse / axis position
	void GetMouseMotion(int& x, int& y) const { x = mouseMotionX; y = mouseMotionY; }
	void GetMousePosition(int& x, int& y) const { x = mouseX; y = mouseY; }
	int GetMouseWheel() const { return mouseWheel; }

public:
	bool close = false;

private:
	KEY_STATE* keyboard;
	KEY_STATE mouseButtons[NUM_MOUSE_BUTTONS];

	int mouseMotionX = 0;
	int mouseMotionY = 0;

	int mouseX = 0;
	int mouseY = 0;

	int mouseWheel = 0;
};