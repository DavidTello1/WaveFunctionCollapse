#pragma once

class Camera;

class CameraController
{
public:
	CameraController(Camera* camera, float moveSpeed, float zoomSpeed) : camera(camera), moveSpeed(moveSpeed), zoomSpeed(zoomSpeed) {}
	~CameraController() {}																 

	void Update(float dt);

	float GetMoveSpeed() const { return moveSpeed; }
	float GetZoomSpeed() const { return zoomSpeed; }

	void SetMoveSpeed(float speed) { moveSpeed = speed; }
	void SetZoomSpeed(float speed) { zoomSpeed = speed; }

private:
	Camera* camera = nullptr;

	float moveSpeed = 0.0f;
	float zoomSpeed = 0.0f;
};