#pragma once

typedef unsigned __int64 uint64;

class PerfTimer
{
public:
	PerfTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	uint64	started_at;
	static uint64 frequency;
};
