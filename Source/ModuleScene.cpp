#include "ModuleScene.h"

#include "Globals.h"

#include "SceneMapGenerator.h"
#include "SceneTileManager.h"

#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Start()
{
	// Scenes
	scenes.append(new SceneMapGenerator());
	scenes.append(new SceneTileManager());

	// Current Scene
	LoadScene(scenes.at(1), true);

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

	for (List<Scene*>::Iterator it = scenes.begin(); it != scenes.end(); ++it)
	{
		(*it)->CleanUp();
		delete (*it);
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
		scenes.append(scene);
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

	Scene* scene = scenes.at(index);
	ret = scene->CleanUp();

	if (!ret)
		LOG("Unable to Unload Scene, error at CleanUp: %s", scene->GetName());

	ret = scenes.erase(scene);

	if (!ret)
		LOG("Unable to Unload Scene, error erasing from list: %d", index);

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

	currentScene = scenes.at(index);
}

int ModuleScene::FindScene(const char* name)
{
	if (scenes.empty())
		return -1;

	int count = 0;
	for (List<Scene*>::Iterator it = scenes.begin(); it != scenes.end(); ++it)
	{
		if ((*it)->GetName() == name) //*** ACCESS BY NAME ? (SHOULD BE UUID)
			return count;

		++count;
	}

	return -1;
}
