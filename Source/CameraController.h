#pragma once

class Camera;

class CameraController
{
public:
	CameraController(Camera* camera, float moveSpeed, float zoomSpeed) : camera(camera), moveSpeed(moveSpeed), zoomSpeed(zoomSpeed) {}
	~CameraController() {}																 

	void Update(float dt);

	void SetMoveSpeed(float speed) { moveSpeed = speed; }
	void SetZoomSpeed(float speed) { zoomSpeed = speed; }

private:
	Camera* camera = nullptr;

	float zoom = 0.0f;

	float moveSpeed = 0.0f;
	float zoomSpeed = 0.0f;
};