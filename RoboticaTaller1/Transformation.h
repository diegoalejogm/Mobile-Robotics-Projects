#pragma once
#include "Point2D.h"

//struct for representing the transformation
struct Transformation{
	Point2D translation;
	prec rotation;
	prec matrix[2][2];

	//_translation is a Point2D with the translation vector
	//_rotation represents the angle (in radians, for deg, multiply by PI/180)
	Transformation(const Point2D& _translation, const prec& _rotation);
	Transformation();

	//Applies the transformation to a point, returning another point.
	Point2D apply(const Point2D& input);
	bool operator== (const Transformation &e1) const;
};
