#include "SceneMapGenerator.h"
#include "Event.h"
#include "SceneEvents.h"

#include "Application.h"
#include "ModuleEvent.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"

#include "Camera.h"
#include "CameraController.h"
#include "ButtonGrid.h"
#include "UI_Button.h"

#include "SDL/include/SDL_scancode.h"

#include "mmgr/mmgr.h"

SceneMapGenerator::SceneMapGenerator()
{
}

SceneMapGenerator::~SceneMapGenerator()
{
}

bool SceneMapGenerator::Init()
{
	camera = new Camera(glm::vec3(0.0f), 0.0f, 1.0f);
	controller = new CameraController(camera, 0.3f);

	state = State::STOP;

	// --- Events
	App->event->Subscribe(this, &SceneMapGenerator::OnWindowResize);
	App->event->Subscribe(this, &SceneMapGenerator::OnZoom);
	App->event->Subscribe(this, &SceneMapGenerator::OnPlay);
	App->event->Subscribe(this, &SceneMapGenerator::OnStep);
	App->event->Subscribe(this, &SceneMapGenerator::OnStop);
	App->event->Subscribe(this, &SceneMapGenerator::OnFinish);
	App->event->Subscribe(this, &SceneMapGenerator::OnSetCell);
	App->event->Subscribe(this, &SceneMapGenerator::OnPresetCells);
	App->event->Subscribe(this, &SceneMapGenerator::OnResetCells);
	App->event->Subscribe(this, &SceneMapGenerator::OnResetAllCells);
	App->event->Subscribe(this, &SceneMapGenerator::OnMapResize);
	App->event->Subscribe(this, &SceneMapGenerator::OnSpacingChange);
	App->event->Subscribe(this, &SceneMapGenerator::OnSaveTileset);

    return true;
}

bool SceneMapGenerator::Start(int width, int height, int cellSize, int spacing, int panelX, int panelY, int panelWidth, int panelHeight)
{
	// --- Button Grid --- 
	buttonGrid = new ButtonGrid(0, 0, width, height, cellSize, spacing, ButtonGrid::Type::MULTIPLE_SELECTION);
	UpdateButtonGrid();

	// --- Background Button ---
	Color transparent = { 0,0,0,0 };
	bgButton = new UI_Button(0, panelY, panelX, App->window->GetHeight(), transparent, transparent, transparent, true);

	// --- Block Button for Panel ---
	blockButton = new UI_Button(panelX, panelY, panelWidth, panelHeight, transparent, transparent, transparent, true);

	return true;
}

bool SceneMapGenerator::PreUpdate(float dt)
{
	// --- Shortcuts
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) // Step
	{
		App->event->Publish(new EventStep());
	}
	else if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) // Play
	{
		if (state == State::FINISHED) // Restart
			App->event->Publish(new EventStop());

		App->event->Publish(new EventPlay());
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) // Stop
	{
		App->event->Publish(new EventStop());
	}

	return true;
}

bool SceneMapGenerator::Update(float dt)
{
	controller->Update(dt);

	if (!blockButton->IsHovered())
		buttonGrid->Update(dt);

	bgButton->Update(dt);

	if (!buttonGrid->IsHovered() && bgButton->IsClicked() &&
		App->input->GetKey(SDL_SCANCODE_RCTRL) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT &&
		App->input->GetKey(SDL_SCANCODE_RALT) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
	{
		buttonGrid->UnSelectAll();
	}

	return true;
}

bool SceneMapGenerator::CleanUp()
{
	delete camera;
	delete controller;

	delete buttonGrid;
	delete bgButton;
	delete blockButton;

    return true;
}

bool SceneMapGenerator::Draw()
{
	buttonGrid->Draw();

	return true;
}

// -------------------------------
// --- EVENTS ---
void SceneMapGenerator::OnWindowResize(EventWindowResize* e)
{
	UpdateButtonGrid();

	camera->UpdateProjectionMatrix(0.0f, e->width * camera->GetZoom(), e->height * camera->GetZoom(), 0.0f);
}

void SceneMapGenerator::OnZoom(EventCameraZoom* e)
{
	float zoom = camera->GetZoom();
	zoom += (e->zoom > 0) ? -controller->GetZoomSpeed() : controller->GetZoomSpeed();

	controller->SetZoom(zoom);
}

void SceneMapGenerator::OnPanelResize(EventPanelResize* e)
{
	bgButton->SetSize(e->x, e->height);

	blockButton->SetPosX(e->x);
	blockButton->SetSize(e->width, e->height);
}

void SceneMapGenerator::OnPlay(EventPlay* e)
{
	if (state != State::STOP && state != State::PAUSE)
		return;
	state = State::PLAY;

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(ButtonGrid::Type::SINGLE_SELECTION);
}

void SceneMapGenerator::OnStep(EventStep* e)
{
	if (state != State::STOP && state != State::PAUSE)
		return;
	state = State::PAUSE;

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(ButtonGrid::Type::SINGLE_SELECTION);
}

void SceneMapGenerator::OnStop(EventStop* e)
{
	if (state == State::STOP)
		return;
	state = State::STOP;

	buttonGrid->UnSelectAll();
	buttonGrid->SetSelectionType(ButtonGrid::Type::MULTIPLE_SELECTION);
}

void SceneMapGenerator::OnFinish(EventFinish* e)
{
	state = State::FINISHED;
}

void SceneMapGenerator::OnSetCell(EventSetCell* e)
{
	buttonGrid->UnSelectAll();
}

void SceneMapGenerator::OnPresetCells(EventPresetCells* e)
{
	buttonGrid->UnSelectAll();
}

void SceneMapGenerator::OnResetCells(EventResetCells* e)
{
	buttonGrid->UnSelectAll();
}

void SceneMapGenerator::OnResetAllCells(EventResetAllCells* e)
{
	buttonGrid->UnSelectAll();
}

void SceneMapGenerator::OnMapResize(EventMapResize* e)
{
	buttonGrid->Resize(e->width, e->height);
	UpdateButtonGrid();
}

void SceneMapGenerator::OnSpacingChange(EventSpacingChange* e)
{
	buttonGrid->SetSpacing(e->spacing);
	UpdateButtonGrid();
}

void SceneMapGenerator::OnSaveTileset(EventSaveTileset* e)
{
	state = State::STOP;
}

// -------------------------------
const unsigned int SceneMapGenerator::GetNumSelected() const
{ 
	return buttonGrid->GetSelected().size();
}

const List<unsigned int> SceneMapGenerator::GetSelected() const 
{
	return buttonGrid->GetSelected(); 
}

void SceneMapGenerator::UpdateButtonGrid()
{
	int width = buttonGrid->GetNumColumns();
	int height = buttonGrid->GetNumRows();
	int spacing = buttonGrid->GetSpacing();
	int buttonSize = buttonGrid->GetButtonSize();

	int offsetX = ((int)App->window->GetWidth() - ((width + spacing) * buttonSize)) / 2;
	int offsetY = ((int)App->window->GetHeight() - ((height + spacing) * buttonSize)) / 2;

	buttonGrid->SetPos(offsetX, offsetY);
}
