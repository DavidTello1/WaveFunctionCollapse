#pragma once
#include "List.h"

class Camera;
class CameraController;
class ButtonGrid;
class UI_Button;

// --- Events
struct EventWindowResize;
struct EventCameraZoom;
struct EventPanelResize;
struct EventPlay;
struct EventStep;
struct EventStop;
struct EventFinish;
struct EventSetCell;
struct EventPresetCells;
struct EventResetCells;
struct EventResetAllCells;
struct EventMapResize;
struct EventSpacingChange;
// ---

class SceneMapGenerator
{
public:
	enum State {
		STOP = 0,
		PLAY,
		PAUSE,
		FINISHED
	};

public:
	SceneMapGenerator();
	~SceneMapGenerator();

	bool Init();
	bool Start(int width, int height, int cellSize, int spacing, int panelX, int panelY, int panelWidth, int panelHeight);
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool CleanUp();

	bool Draw();

	// --- Utils
	Camera* GetCamera() const { return camera; }
	const State& GetState() const { return state; }
	const unsigned int GetNumSelected() const;
	const List<unsigned int> GetSelected() const;

private:
	void UpdateButtonGrid();

	// --- Events
	void OnWindowResize(EventWindowResize* e);
	void OnZoom(EventCameraZoom* e);

	void OnPanelResize(EventPanelResize* e);

	void OnPlay(EventPlay* e);
	void OnStep(EventStep* e);
	void OnStop(EventStop* e);
	void OnFinish(EventFinish* e);

	void OnSetCell(EventSetCell* e);
	void OnPresetCells(EventPresetCells* e);
	void OnResetCells(EventResetCells* e);
	void OnResetAllCells(EventResetAllCells* e);

	void OnMapResize(EventMapResize* e);
	void OnSpacingChange(EventSpacingChange* e);

private:
	State state = State::STOP;

	Camera* camera = nullptr;
	CameraController* controller = nullptr;

	ButtonGrid* buttonGrid = nullptr;
	UI_Button* bgButton = nullptr;
	UI_Button* blockButton = nullptr;
};
