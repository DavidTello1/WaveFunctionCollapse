#pragma once

#include "SDL\include\SDL_timer.h"

class Timer
{
public:
	Timer() {
		Start();
	}

	void Start() {
		running = true;
		started_at = SDL_GetTicks();
	}

	void Stop() {
		running = false;
		stopped_at = SDL_GetTicks();
	}

	Uint32 Read() const {
		if (running == true)
			return SDL_GetTicks() - started_at;
		else
			return stopped_at - started_at;
	}

	float ReadSec() const
	{
		return float(SDL_GetTicks() - started_at) / 1000.0f;
	}

private:
	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
};
