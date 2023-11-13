#pragma once

#include "glm/include/glm/glm.hpp"

class Camera
{
	friend class CameraController;
public:
	Camera();
	Camera(glm::vec3 position, float rotation, float zoom, float minZoom, float maxZoom);
	~Camera() {};

	// --- Getters
	const glm::vec3& GetPosition() const { return position; }
	const float& GetRotation() const { return rotation; }
	const float& GetZoom() const { return zoom; }
	const float& GetMinZoom() const { return minZoom; }
	const float& GetMaxZoom() const { return maxZoom; }

	// --- Setters
	void SetPosition(glm::vec3 pos) { position = pos; UpdateViewMatrix(); }
	void SetRotation(float rot) { rotation = rot; UpdateViewMatrix(); }
	void SetZoom(float value);
	void SetZoomLimits(float min, float max) { minZoom = min; maxZoom = max; }

	// --- ViewProj
	void UpdateProjectionMatrix(float left, float right, float top, float bottom);
	const glm::mat4& GetViewProjMatrix() const { return ViewProjMatrix; }
	const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
	const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }

protected:
	void UpdateViewMatrix();

protected:
	glm::vec3 position = glm::vec3(0.0f);
	float rotation = 0.0f;
	float zoom = 0.0f;

	float minZoom = 0.0f;
	float maxZoom = 0.0f;

	glm::mat4 ViewMatrix = glm::mat4(1.0f);
	glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 ViewProjMatrix = glm::mat4(1.0f);
};
