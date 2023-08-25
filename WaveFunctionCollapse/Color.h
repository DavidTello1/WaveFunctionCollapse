#pragma once

struct Color {
public:
	Color() : r(0), g(0), b(0), a(0) {};
	Color(const unsigned int r, const unsigned int g, const unsigned int b, const unsigned int a) : r(r), g(g), b(b), a(a) {}
	~Color() {}

public:
	unsigned int r;
	unsigned int g;
	unsigned int b;
	unsigned int a;
};