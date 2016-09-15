#pragma once

#include "Point2D.h"
//Struct for using integer points, this will avoid repeating points
struct IntegerPoint2D {
	int x, y;
	IntegerPoint2D(const int &_x, const int &_y);
	IntegerPoint2D(const Point2D &_point);
	IntegerPoint2D();
	IntegerPoint2D operator+ (const IntegerPoint2D &e1) const;
	bool operator< (const IntegerPoint2D &e1) const;
};