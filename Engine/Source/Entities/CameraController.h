#pragma once

class Camera;

class CameraController
{
public:
	CameraController(Camera* camera, float zoomSpeed, float minZoom = 1.0f, float maxZoom = 10.0f) 
		: camera(camera), zoomSpeed(zoomSpeed), minZoom(minZoom), maxZoom(maxZoom) {}
	~CameraController() {}																 

	void Update(float dt);

	// --- Getters
	const float& GetZoomSpeed() const { return zoomSpeed; }
	const float& GetMinZoom() const { return minZoom; }
	const float& GetMaxZoom() const { return maxZoom; }

	// --- Setters
	void SetZoomSpeed(float speed) { zoomSpeed = speed; }
	void SetZoomLimits(float min, float max) { minZoom = min; maxZoom = max; }
	void SetZoom(float value);

private:
	void OnPanning();

private:
	Camera* camera = nullptr;

	// Zoom
	float zoomSpeed = 0.0f;
	float minZoom = 0.0f;
	float maxZoom = 0.0f;

	// Pan
	float lastX = 0.0f;
	float lastY = 0.0f;
};
