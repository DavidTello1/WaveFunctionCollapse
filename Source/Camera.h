#pragma once

#include "glm/include/glm/glm.hpp"

struct EventWindowResize;

class Camera
{
	friend class CameraController;
public:
	Camera();
	Camera(glm::vec3 position, float rotation, float zoom);
	~Camera() {};

	// --- Getters
	const glm::vec3& GetPosition() const { return position; }
	const float& GetRotation() const { return rotation; }
	const float& GetZoom() const { return zoom; }

	// --- Setters
	void SetPosition(glm::vec3 pos) { position = pos; UpdateViewMatrix(); }
	void SetRotation(float rot) { rotation = rot; UpdateViewMatrix(); }
	void SetZoom(float value);

	// --- Matrices
	const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
	const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }
	const glm::mat4& GetViewProjMatrix() const { return ViewProjMatrix; }

	// --- Events
	void OnWindowResize(EventWindowResize* e);

protected:
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float left, float right, float top, float bottom);

protected:
	glm::vec3 position = glm::vec3(0.0f);
	float rotation = 0.0f;
	float zoom = 0.0f;

	glm::mat4 ViewMatrix = glm::mat4(1.0f);
	glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 ViewProjMatrix = glm::mat4(1.0f);
};
