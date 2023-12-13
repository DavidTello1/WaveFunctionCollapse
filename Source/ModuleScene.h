#pragma once
#include "Module.h"

#include <vector>

class Scene;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	virtual ~ModuleScene();

	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	void Draw();

	// --- Scene Management
	bool LoadScene(Scene* scene, bool active = false);
	bool UnLoadScene(const char* name);

	// --- Utils
	Scene* GetCurrentScene() const { return currentScene; }
	void SetCurrentScene(Scene* scene) { currentScene = scene; }
	void SetCurrentScene(const char* name);

private:
	int FindScene(const char* name); // Returns index to scene or -1 if not found
	
private:
	Scene* currentScene = nullptr;
	std::vector<Scene*> scenes;
};
