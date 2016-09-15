#pragma once
#include "Aria.h"
#include "IntegerPoint2D.h"
#include "Transformation.h"
class SensorDataTask
{
public:
	// Constructor. Adds our 'user task' to the given robot object.
	SensorDataTask(ArRobot *robot);

	// Destructor. Removes our user task from the robot
	~SensorDataTask(void);

	// This method will be called by the callback functor
	void doTask(void);
protected:
	FILE * robotFrameOutputFile;
	FILE * sceneFrameOutputFile;
	FILE * initialRobotPosition;

	Transformation sceneTransformation;

	ArRobot *myRobot;
	Transformation lastKnownLocation;

	std::set<IntegerPoint2D> knownPoints;

	// The functor to add to the robot for our 'user task'.
	ArFunctorC<SensorDataTask> myTaskCB;
};