#include "TransformationUtil.h"
#include "Transformation.h"
#include <list>
#include "Aria.h"
Transformation getTransformation(double X, double Y, double degree){
	Transformation t = Transformation(Point2D(X,Y), degree * PI / 180.0);
	return t;
}

Point2D transformPoint(Transformation& t,Point2D& point)
{
	Point2D s = t.apply(point);
	return s;
}

std::vector<Point2D*> transformPoses(std::list<ArPoseWithTime*>& poses, Transformation& t)
{
	std::vector<Point2D*> transformed;
	for (std::list<ArPoseWithTime*>::iterator it = poses.begin(); it != poses.end(); ++it){
		ArPoseWithTime* posei = *it;
		Point2D newP = Point2D((*posei).getX(), (*posei).getY());
		transformed.push_back(&transformPoint(t, newP));
	}
	return transformed;
}