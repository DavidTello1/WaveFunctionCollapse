#include "CameraController.h"
#include "Camera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#include "SDL/include/SDL_scancode.h"

void CameraController::Update(float dt)
{
	bool isChanged = false;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT) // Left
	{
		camera->position.x -= moveSpeed * dt;
		isChanged = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT) // Right
	{
		camera->position.x += moveSpeed * dt;
		isChanged = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_REPEAT) // Up
	{
		camera->position.y -= moveSpeed * dt;
		isChanged = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_REPEAT) // Down
	{
		camera->position.y += moveSpeed * dt;
		isChanged = true;
	}

	// --- Update Camera ViewMatrix
	if (isChanged)
	{
		camera->UpdateViewMatrix();
	}
}
