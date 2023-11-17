#include "Camera.h"

#include "Application.h"
#include "ModuleWindow.h"

#include "glm/include/glm/gtc/type_ptr.hpp"

Camera::Camera()
{
	position = glm::vec3(0.0f);
	rotation = 0.0f;
	zoom = 1.0f;
	UpdateProjectionMatrix(0, (float)App->window->GetWidth() * zoom, (float)App->window->GetHeight() * zoom, 0);
	UpdateViewMatrix();
}

Camera::Camera(glm::vec3 position, float rotation, float zoom) : position(position), rotation(rotation), zoom(zoom)
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
	zoom = value;
	UpdateProjectionMatrix(0.0f, (float)App->window->GetWidth() * zoom, (float)App->window->GetHeight() * zoom, 0.0f);
}
