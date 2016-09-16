#include "Point2D.h"
#include<cmath>
#define PI acos(-1.0)
#define EPS 1e-6


Point2D::Point2D(){
	x = 0;
	y = 0;
}

Point2D::Point2D(const prec &_x, const prec &_y) {
	x = _x;
	y = _y;
}

Point2D Point2D::operator+ (const Point2D &e1) const
{
	return Point2D(x + e1.x, y + e1.y);
}

bool Point2D::operator== (const Point2D &e1) const {
	return (abs(x - e1.x) < EPS) && (abs(y - e1.y) < EPS);
}

prec Point2D::dist(const Point2D & e1)
{
	return sqrt((e1.x - x)*(e1.x - x) + (e1.y - y)*(e1.y - y));
}
