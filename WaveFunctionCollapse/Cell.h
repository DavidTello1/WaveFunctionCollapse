#pragma once
#include "BitMask.h"

struct Cell 
{
public:
	Cell(int index, unsigned int maskSize) {
		tileID = -1;
		isCollapsed = false;

		this->index = index;
		this->mask = new BitMask(maskSize);
		this->mask->SetAll();
	};

	~Cell() {
		delete mask;
		mask = nullptr;
	}

	int GetEntropy() const {
		return mask->Count();
	}

	void Update() {
		if (mask->Count() == 1)
			Observe();
	}

	void Observe() {
		tileID = mask->Pick();

		if (tileID != -1)
			isCollapsed = true;
		else
			isInvalid = true;
	}

public:
	int index = -1;
	int tileID = -1;
	BitMask* mask;
	bool isCollapsed = false;
	bool isInvalid = false;
};
