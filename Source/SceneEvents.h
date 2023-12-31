#pragma once
#include "Event.h"

#include "String.h"
#include "List.h"

// -------------------------------------
struct EventPlay : public Event {};
struct EventStep : public Event {};
struct EventStop : public Event {};

struct EventSetCell : public Event {
	EventSetCell(unsigned int cell, int tileID) : cell(cell), tileID(tileID) {}
	unsigned int cell;
	int tileID;
};

struct EventPresetCells : public Event {
	EventPresetCells(List<unsigned int> cells, int tileID) : cells(cells), tileID(tileID) {}
	List<unsigned int> cells;
	int tileID;
};

struct EventResetCells : public Event {
	EventResetCells(List<unsigned int> cells) : cells(cells) {}
	List<unsigned int> cells;
};

struct EventResetAllCells : public Event {};

struct EventMapResize : public Event {
	EventMapResize(int w, int h) : width(w), height(h) {}
	int width, height;
};

// -------------------------------------
struct EventImport : public Event {};
struct EventSaveTileset : public Event {};

struct EventRemoveTile : public Event {
	EventRemoveTile(int index) : index(index) {}
	int index;
};

struct EventUpdateMask : public Event {
	EventUpdateMask(int index, int dir, int bit, bool value) : index(index), dir(dir), bit(bit), value(value) {}
	int index, dir, bit;
	bool value;
};
