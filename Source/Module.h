#pragma once

class Module
{
public:
	Module(bool start_enabled = true) : enabled(start_enabled) {}
	virtual ~Module() {}

	virtual bool Init() { return true; }
	virtual bool Start() { return true; }
	virtual bool PreUpdate(float dt) { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate(float dt) { return true; }
	virtual bool CleanUp() { return true; }

	bool IsEnabled() const { return enabled; }

	void SetEnabled(bool active)
	{
		if (enabled == active)
			return;

		enabled = active;
		(active == true) ? Start() : CleanUp();
	}

private:
	bool enabled;
};