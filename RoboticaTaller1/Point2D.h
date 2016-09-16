#pragma once

//precision to be used, by default we use double
typedef double prec;
//Struct for using vectors in 2D, comes with addition
struct Point2D{
	prec x, y;
	Point2D(const prec &_x, const prec &_y);
	Point2D();
	
	Point2D operator+ (const Point2D &e1) const;
	bool operator== (const Point2D &e1) const;
	prec dist(const Point2D &e1);
};