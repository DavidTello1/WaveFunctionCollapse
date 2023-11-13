#include "Camera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#include "glm/include/glm/gtc/type_ptr.hpp"

Camera::Camera()
{
	position = glm::vec3(0.0f);
	rotation = 0.0f;
	zoom = 1.0f;
	minZoom = 1.0f;
	maxZoom = 10.0f;
	UpdateProjectionMatrix(0, (float)App->window->GetWidth() * zoom, (float)App->window->GetHeight() * zoom, 0);
	UpdateViewMatrix();
}

Camera::Camera(glm::vec3 position, float rotation, float zoom, float minZoom, float maxZoom) :
	position(position), rotation(rotation), zoom(zoom), minZoom(minZoom), maxZoom(maxZoom)
{
	UpdateProjectionMatrix(0, (float)App->window->GetWidth()* zoom, (float)App->window->GetHeight()* zoom, 0);
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));

	ViewMatrix = glm::inverse(transform);
	ViewProjMatrix = ProjectionMatrix * ViewMatrix;
}

void Camera::UpdateProjectionMatrix(float left, float right, float top, float bottom)
{
	ProjectionMatrix = glm::ortho(left, right, top, bottom);
	ViewProjMatrix = ProjectionMatrix * ViewMatrix;
}

void Camera::SetZoom(float value)
{
	if (value > maxZoom)
		value = maxZoom;
	else if (value < minZoom)
		value = minZoom;

	if (value == zoom)
		return;

	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);

	// Apply Offset to Camera Position (Center in Mouse)
	position.x += mouseX * (zoom - value);
	position.y += mouseY * (zoom - value);

	zoom = value;
	ProjectionMatrix = glm::ortho(0.0f, (float)App->window->GetWidth() * zoom, (float)App->window->GetHeight() * zoom, 0.0f);
	UpdateViewMatrix();
}
