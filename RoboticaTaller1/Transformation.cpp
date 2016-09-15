#include "Transformation.h"
#include "Point2D.h"

Transformation::Transformation(const Point2D& _translation, const prec& _rotation){
	translation = Point2D(_translation.x, _translation.y);
	rotation = _rotation;
	matrix[0][0] = cos(rotation);
	matrix[0][1] = -1 * sin(rotation);
	matrix[1][0] = sin(rotation);
	matrix[1][1] = cos(rotation);
}
//Applies the transformation to a point, returning another point.
Point2D Transformation::apply(const Point2D& input){
	Point2D res = Point2D(input.x*matrix[0][0] + input.y*matrix[0][1], input.x*matrix[1][0] + input.y*matrix[1][1]);
	res = res + translation;
	return res;
}