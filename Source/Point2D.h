#pragma once

#include <math.h> // for sqrt (distance between points)

struct Point2D
{
public:
	Point2D(int x, int y) : x(x), y(y) {};


	// --- OPERATORS ---
	Point2D operator -(const Point2D& v) const {
		return Point2D(x - v.x, y - v.y);
	}

	Point2D operator +(const Point2D& v) const {
		return Point2D(x + v.x, y + v.y);
	}

	const Point2D& operator -=(const Point2D& v) {
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const Point2D& operator +=(const Point2D& v) {
		x += v.x;
		y += v.y;

		return(*this);
	}

	bool operator ==(const Point2D& v) const {
		return (x == v.x && y == v.y);
	}

	bool operator !=(const Point2D& v) const {
		return (x != v.x || y != v.y);
	}


	// --- UTILS ---
	int DistanceTo(const Point2D& p) const
	{
		int fx = x - p.x;
		int fy = y - p.y;

		return (int)sqrtf((float)((fx * fx) + (fy * fy)));
	}

public:
	int x, y;
};
