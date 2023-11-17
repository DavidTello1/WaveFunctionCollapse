#pragma once

class Camera;

class CameraController
{
public:
	CameraController(Camera* camera, float moveSpeed, float zoomSpeed, float minZoom = 1.0f, float maxZoom = 10.0f) 
		: camera(camera), moveSpeed(moveSpeed), zoomSpeed(zoomSpeed), minZoom(minZoom), maxZoom(maxZoom) {}
	~CameraController() {}																 

	void Update(float dt);

	// --- Getters
	const float& GetMoveSpeed() const { return moveSpeed; }
	const float& GetZoomSpeed() const { return zoomSpeed; }
	const float& GetMinZoom() const { return minZoom; }
	const float& GetMaxZoom() const { return maxZoom; }

	// --- Setters
	void SetMoveSpeed(float speed) { moveSpeed = speed; }
	void SetZoomSpeed(float speed) { zoomSpeed = speed; }
	void SetZoomLimits(float min, float max) { minZoom = min; maxZoom = max; }
	void SetZoom(float value);

private:
	void OnPanning();

private:
	Camera* camera = nullptr;

	float moveSpeed = 0.0f;

	float zoomSpeed = 0.0f;
	float minZoom = 0.0f;
	float maxZoom = 0.0f;

	// Panning
	float lastX = 0.0f;
	float lastY = 0.0f;
};
