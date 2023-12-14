#pragma once
#include "Event.h"

#include "String.h"
#include "List.h"

struct Tile;

// -------------------------------------
struct EventOpenImport : public Event {};
struct EventImportTile : public Event
{
	EventImportTile(Tile* tile) : tile(tile) {}
	Tile* tile;
};
struct EventImportTileset : public Event {};
struct EventImportMap : public Event {};
struct EventExportTileset : public Event {};
struct EventExportMap : public Event {};

// -------------------------------------
struct EventPlay : public Event {};
struct EventStep : public Event {};
struct EventStop : public Event {};
struct EventFinish : public Event {};

struct EventSetCell : public Event
{
	EventSetCell(unsigned int cell, int tileID) : cell(cell), tileID(tileID) {}
	unsigned int cell;
	int tileID;
};

struct EventPresetCells : public Event
{
	EventPresetCells(List<unsigned int> cells, int tileID) : cells(cells), tileID(tileID) {}
	List<unsigned int> cells;
	int tileID;
};

struct EventResetCells : public Event
{
	EventResetCells(List<unsigned int> cells) : cells(cells) {}
	List<unsigned int> cells;
};

struct EventResetAllCells : public Event {};

struct EventMapResize : public Event
{
	EventMapResize(int w, int h) : width(w), height(h) {}
	int width, height;
};

struct EventPanelToggled : public Event
{
	EventPanelToggled(bool isOpen) : isOpen(isOpen) {}
	bool isOpen;
};

struct EventPanelResize : public Event
{
	EventPanelResize(int x, int w, int h) : x(x), width(w), height(h) {}
	int x, width, height;
};

struct EventDrawTextures : public Event
{
	EventDrawTextures(bool value) : value(value) {}
	bool value;
};

struct EventDrawSpaced : public Event
{
	EventDrawSpaced(bool value) : value(value) {}
	bool value;
};

struct EventSpacingChange : public Event
{
	EventSpacingChange(int spacing) : spacing(spacing) {}
	int spacing;
};

struct EventChangeScene : public Event
{
	EventChangeScene(const char* scene) : scene(scene) {} //*** PROBABLY WONT WORK WITH CONST CHAR*
	String scene;
};

struct EventTimerChange : public Event
{
	EventTimerChange(float step, float total) : stepTime(step), totalTime(total) {}
	float stepTime, totalTime;
};

// -------------------------------------
struct EventSaveTileset : public Event {};
