#pragma once
#include <string> //***

#define MODULE_NAME_LENGTH 25

class Camera;

class Scene
{
public:
	Scene(const char* name) { strcpy_s(this->name, MODULE_NAME_LENGTH, name); }
	virtual ~Scene() {}

	virtual bool Init() { return true; }
	virtual bool Start() { return true; }
	virtual bool PreUpdate(float dt) { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate(float dt) { return true; }
	virtual bool CleanUp() { return true; }

	virtual bool Draw() { return true; }
	virtual bool DrawUI() { return true; }

	//virtual bool Load(const File& file) { return true; }
	//virtual bool Save(File* file) { return true; }

	// --- Utils
	const char* GetName() const { return name; }
	Camera* GetCamera() { return camera; }

protected:
	char name[MODULE_NAME_LENGTH];

	Camera* camera = nullptr;
};
