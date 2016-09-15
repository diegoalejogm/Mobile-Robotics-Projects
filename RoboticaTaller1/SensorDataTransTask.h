#pragma once
#include "Aria.h"
class SensorDataTransTask
{
public:
	// Constructor. Adds our 'user task' to the given robot object.
	SensorDataTransTask(ArRobot *robot);

	// Destructor. Removes our user task from the robot
	~SensorDataTransTask(void);

	// This method will be called by the callback functor
	void doTask(void);
protected:
	ArRobot *myRobot;

	// The functor to add to the robot for our 'user task'.
	ArFunctorC<SensorDataTransTask> myTaskCB;
};