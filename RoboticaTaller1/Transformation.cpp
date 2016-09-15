#include<cmath>
#include<iostream>
#include "Transformation.h"
#include "Point2D.h"
#define EPS 1e-6

//_translation is a Point2D with the translation vector
//_rotation represents the angle (in radians, for deg, multiply by PI/180)
Transformation::Transformation(const Point2D& _translation, const prec& _rotation) {
	translation = Point2D(_translation.x, _translation.y);
	rotation = _rotation;
	matrix[0][0] = cos(rotation);
	matrix[0][1] = -1 * sin(rotation);
	matrix[1][0] = sin(rotation);
	matrix[1][1] = cos(rotation);
}
Transformation::Transformation() {
	translation = Point2D(0, 0);
	matrix[0][0] = matrix[1][1] = 1;
	matrix[0][1] = matrix[1][0] = 0;
}
//Applies the transformation to a point, returning another point.
Point2D Transformation::apply(const Point2D& input) {
	Point2D res = Point2D(input.x*matrix[0][0] + input.y*matrix[0][1], input.x*matrix[1][0] + input.y*matrix[1][1]);
	res = res + translation;
	return res;
}
bool Transformation::operator== (const Transformation &e1) const {
	return (e1.translation == translation) && (abs(matrix[0][0] - e1.matrix[0][0]) < EPS && abs(matrix[0][1] - e1.matrix[0][1]) < EPS);
}

