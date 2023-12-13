#include "ModuleScene.h"
#include "Globals.h"

#include "MainScene.h"

#include <string>

#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Start()
{
	// Main Scene
	LoadScene(new MainScene(), true);

	return true;
}

bool ModuleScene::Update(float dt)
{
	if (currentScene != nullptr)
		currentScene->Update(dt);

	return true;
}

bool ModuleScene::CleanUp()
{
	LOG("Deleting scenes");

	for (size_t i = 0; i < scenes.size(); ++i)
	{
		scenes[i]->CleanUp();
		delete scenes[i];
	}
	scenes.clear();

	return true;
}

void ModuleScene::Draw()
{
	if (currentScene != nullptr)
		currentScene->Draw();
}

// --------------------------------
bool ModuleScene::LoadScene(Scene* scene, bool active)
{
	bool ret = false;

	int index = FindScene(scene->GetName());
	if (index == -1)
	{
		scenes.push_back(scene);
	}

	ret = scene->Init();
	if (ret)
		ret = scene->Start();

	if (ret && active)
		currentScene = scene;

	return ret;
}

bool ModuleScene::UnLoadScene(const char* name)
{
	bool ret = false;

	int index = FindScene(name);
	if (index == -1)
	{
		LOG("Unable to Unload Scene, name not found: %s", name);
		return false;
	}

	Scene* scene = scenes[index];
	ret = scene->CleanUp();

	if (!ret)
		LOG("Unable to Unload Scene, error at CleanUp: %s", scene->GetName());

	scenes.erase(scenes.begin() + index);

	return ret;
}

void ModuleScene::SetCurrentScene(const char* name)
{
	int index = FindScene(name);
	if (index == -1)
	{
		LOG("Unable to Change Current Scene, name not found: %s", name);
		return;
	}

	currentScene = scenes[index];
}

int ModuleScene::FindScene(const char* name)
{
	if (scenes.empty())
		return -1;

	int count = 0;
	for (size_t i = 0; i < scenes.size(); ++i)
	{
		std::string sceneName = scenes[i]->GetName();
		if (sceneName == name)
			return count;

		++count;
	}

	return -1;
}
