#pragma once    // To make sure you don't declare the function more than once by including the header multiple times.
#include "Point2D.h"
#include "Transformation.h"
#include <Aria.h>

Transformation getTransformation(double X, double Y, double degree);
Point2D transformPoint(Transformation& t, Point2D& point);
std::vector<Point2D*> transformPoses(std::list<ArPoseWithTime*>& poses, Transformation& t);
