#include "ModuleScene.h"

#include "Globals.h"

#include "SceneMapGenerator.h"

#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Start()
{
	// Load Scenes
	LoadScene(new SceneMapGenerator(), true);

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

	ListItem<Scene*>* item;
	for (item = scenes.front(); item != nullptr; item = item->next)
	{
		item->data->CleanUp();
		delete item->data;
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
		scenes.add(scene);
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

	Scene* scene = scenes[index]->data;
	ret = scene->CleanUp();

	if (!ret)
		LOG("Unable to Unload Scene, error at CleanUp: %s", scene->GetName());

	ret = scenes.erase(scenes[index]);

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

	currentScene = scenes[index]->data;
}

int ModuleScene::FindScene(const char* name)
{
	int count = 0;

	ListItem<Scene*>* item;
	for (item = scenes.front(); item != nullptr; item = item->next)
	{
		if (item->data->GetName() == name) //***
			return count;

		++count;
	}

	return -1;
}
