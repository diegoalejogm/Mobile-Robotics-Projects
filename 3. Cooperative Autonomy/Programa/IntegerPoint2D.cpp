#include "IntegerPoint2D.h"
#include<cmath>

IntegerPoint2D::IntegerPoint2D(const int &_x, const int &_y) {
	x = _x;
	y = _y;
}

IntegerPoint2D::IntegerPoint2D(const Point2D &_point) {
	x = (int)floor(_point.x);
	y = (int)floor(_point.y);
}

IntegerPoint2D::IntegerPoint2D() {
	x = 0;
	y = 0;
}

IntegerPoint2D IntegerPoint2D::operator + (const IntegerPoint2D &e1) const {
	return IntegerPoint2D(x + e1.x, y + e1.y);
}

bool IntegerPoint2D::operator< (const IntegerPoint2D &e1) const {
	if (x == e1.x) return y < e1.y;
	return x < e1.x;
}