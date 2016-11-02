#pragma once
#include "Aria.h"
class PrintingTask
{
public:
	// Constructor. Adds our 'user task' to the given robot object.
	PrintingTask(ArRobot *robot);

	// Destructor. Removes our user task from the robot
	~PrintingTask(void);

	// This method will be called by the callback functor
	void doTask(void);
protected:
	ArRobot *myRobot;

	// The functor to add to the robot for our 'user task'.
	ArFunctorC<PrintingTask> myTaskCB;
};