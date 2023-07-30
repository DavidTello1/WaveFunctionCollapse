#include "ModuleScene.h"

#include "Application.h"
#include "ModuleRenderer.h"

#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	return true;
}

bool ModuleScene::Start()
{
	return true;
}

bool ModuleScene::Update(float dt)
{
	App->renderer->DrawQuad({ 20, 20, 50, 50 }, 255, 255, 255);

	return true;
}

bool ModuleScene::CleanUp()
{
	return true;
}

void ModuleScene::Draw()
{
}
