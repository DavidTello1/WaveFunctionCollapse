#pragma once

#include "glm/include/glm/glm.hpp"

class Camera
{
	friend class CameraController;
public:
	Camera();
	~Camera() {};

	// --- Getters
	const glm::vec3& GetPosition() const { return position; }
	const float& GetRotation() const { return rotation; }

	// --- Setters
	void SetPosition(glm::vec3 pos) { position = pos; UpdateViewMatrix(); }
	void SetRotation(float rot) { rotation = rot; UpdateViewMatrix(); }

	// --- ViewProj
	void UpdateProjectionMatrix(float left, float right, float top, float bottom);
	const glm::mat4& GetViewProjMatrix() const { return ViewProjMatrix; }

protected:
	void UpdateViewMatrix();

protected:
	glm::vec3 position = glm::vec3(0.0f);
	float rotation = 0.0f;

	glm::mat4 ViewMatrix = glm::mat4(1.0f);
	glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 ViewProjMatrix = glm::mat4(1.0f);
};
