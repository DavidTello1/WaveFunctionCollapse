#pragma once
#include "BitMask.h"

struct Cell 
{
public:
	// --- Constructor
	Cell(int index, unsigned int maskSize) {
		tileID = -1;
		isCollapsed = false;
		isInvalid = false;
		isPreset = false;

		this->index = index;
		this->mask = new BitMask(maskSize);
		this->mask->SetAll();
	};

	// --- Destructor
	~Cell() {
		delete mask;
		mask = nullptr;
	}

	// --- Main Functions
	void Update() {
		if (mask->Count() == 1)
			Observe();
	}

	void Observe() {
		tileID = mask->Pick();
		isCollapsed = true;

		if (tileID == -1)
			isInvalid = true;
	}

	// --- Utils
	int GetEntropy() const {
		return mask->Count();
	}

	void Reset() {
		tileID = -1;
		isCollapsed = false;
		isInvalid = false;
		isPreset = false;
		mask->SetAll();
	}
	
	void SetCell(int tileID) {
		//if (tileID < 0 || tileID > maskSize) //***MASK SIZE
		//	return;

		this->tileID = tileID;
		isCollapsed = true;
		isInvalid = false;
		mask->ClearAll();
		mask->SetBit(tileID, true);
	}

public:
	int index = -1;
	int tileID = -1;
	bool isCollapsed = false;
	bool isInvalid = false;
	bool isPreset = false;
	BitMask* mask;
};
