#include "Point2D.h"


Point2D::Point2D()
{
	x = 0;
	y = 0;
}

Point2D::Point2D(const prec &_x, const prec &_y){
	x = _x;
	y = _y;
}

Point2D Point2D::operator+ (const Point2D &e1) const
{
	return Point2D(x + e1.x, y + e1.y);
}
