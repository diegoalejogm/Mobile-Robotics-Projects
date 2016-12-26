#include "PrintingTask.h"
#include "Aria.h"


// the constructor (note how it uses chaining to initialize myTaskCB)
PrintingTask::PrintingTask(ArRobot *robot) :
myTaskCB(this, &PrintingTask::doTask)
{
	myRobot = robot;
	// just add it to the robot
	myRobot->addSensorInterpTask("PrintingTask", 50, &myTaskCB);
}

PrintingTask::~PrintingTask()
{
	myRobot->remSensorInterpTask(&myTaskCB);
}

void PrintingTask::doTask(void)
{
	// print out some info about the robot
	myRobot->lock();

	ArLog::log(ArLog::Normal, "Robot Coordinates: \n\tx %6.1f    y %6.1f    th %6.1f    vel %7.1f    mpacs %3d \n\t", myRobot->getX(),
		myRobot->getY(), myRobot->getTh(), myRobot->getVel(),
		myRobot->getMotorPacCount());

	myRobot->unlock();

	//ArUtil::sleep(5000);
}
