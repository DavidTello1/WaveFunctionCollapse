#pragma once
#include "BitArray.h"

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
		mask = BitArray(maskSize);
		mask.setAll();
	};

	// --- Destructor
	~Cell() {
	}

	// --- Collapse Cell
	void Observe(int id) {
		tileID = id;
		isCollapsed = true;

		mask.clearAll();

		if (tileID == -1)
			isInvalid = true;
		else
			mask.setBit(id);
	}

	// --- Utils
	int GetEntropy() const {
		return mask.count();
	}

	void Reset() {
		tileID = -1;
		isCollapsed = false;
		isInvalid = false;
		isPreset = false;
		mask.setAll();
	}
	
	void SetCell(int tileID) {
		if (tileID < 0 || tileID > mask.size())
			return;

		this->tileID = tileID;
		isCollapsed = true;
		isInvalid = false;
		mask.clearAll();
		mask.setBit(tileID);
	}

public:
	int index = -1;
	int tileID = -1;
	bool isCollapsed = false;
	bool isInvalid = false;
	bool isPreset = false;
	BitArray mask;
};
