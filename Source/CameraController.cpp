#include "CameraController.h"
#include "Camera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#include "SDL/include/SDL_mouse.h"

void CameraController::Update(float dt)
{
	if (camera == nullptr)
		return;

	OnPanning();
}

void CameraController::OnPanning()
{
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		int mouseX, mouseY;
		App->input->GetMousePosition(mouseX, mouseY);

		lastX = mouseX;
		lastY = mouseY;
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int mouseX, mouseY;
		App->input->GetMousePosition(mouseX, mouseY);

		if (lastX == mouseX && lastY == mouseY)
			return;

		camera->position.x += (lastX - mouseX) * camera->zoom;
		camera->position.y += (lastY - mouseY) * camera->zoom;
		camera->UpdateViewMatrix();

		lastX = mouseX;
		lastY = mouseY;
	}
}

void CameraController::SetZoom(float value)
{
	if (value > maxZoom)
		value = maxZoom;
	else if (value < minZoom)
		value = minZoom;

	if (camera == nullptr || value == camera->zoom)
		return;

	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);

	// Apply Offset to Camera Position (Center in Mouse)
	camera->position.x += mouseX * (camera->zoom - value);
	camera->position.y += mouseY * (camera->zoom - value);

	// Update Camera
	camera->zoom = value;
	camera->UpdateProjectionMatrix(0.0f, (float)App->window->GetWidth() * value, (float)App->window->GetHeight() * value, 0.0f);
	camera->UpdateViewMatrix();
}
