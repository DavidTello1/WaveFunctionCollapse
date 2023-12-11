#pragma once

#include "PCG/pcg_basic.h"
#include <time.h>

class RandomNumber
{
public:
	RandomNumber() {
		pcg32_srandom_r(&rng, time(NULL), (intptr_t)&rng);
	}
	RandomNumber(uint64_t seed) {
		pcg32_srandom_r(&rng, seed, (intptr_t)&rng);
	}

	~RandomNumber() {
	}

	void SetSeed(uint64_t seed) {
		pcg32_srandom_r(&rng, seed, (intptr_t)&rng);
	}

	uint32_t GenerateInt() {
		return pcg32_random_r(&rng);
	}

	uint32_t GenerateBoundedInt(uint32_t bound) {
		return pcg32_boundedrand_r(&rng, bound);
	}

	uint64_t GenerateUID() { //***
		return 0;
	}

private:
	pcg32_random_t rng;
};
