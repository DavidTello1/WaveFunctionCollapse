#pragma once

struct Color {
public:
	Color() : r(0), g(0), b(0), a(0) {};
	Color(const float r, const float g, const float b, const float a) : r(r), g(g), b(b), a(a) {}
	~Color() {}

public:
	float r;
	float g;
	float b;
	float a;
};