#include "SensorDataTransTask.h"
#include "Aria.h"
#include "TransformationUtil.h"
#include <list>
#include <iostream>

// the constructor (note how it uses chaining to initialize myTaskCB)
SensorDataTransTask::SensorDataTransTask(ArRobot *robot) :
myTaskCB(this, &SensorDataTransTask::doTask)
{
	myRobot = robot;
	// just add it to the robot
	myRobot->addSensorInterpTask("DataTransformationTask", 50, &myTaskCB);
}

SensorDataTransTask::~SensorDataTransTask()
{
	myRobot->remSensorInterpTask(&myTaskCB);
}

void SensorDataTransTask::doTask(void)
{

	int numLasers = 0;

	// Get a pointer to ArRobot's list of connected lasers. We will lock the robot while using it to prevent changes by tasks in the robot's background task thread or any other threads. Each laser has an index. You can also store the laser's index or name (laser->getName()) and use that to get a reference (pointer) to the laser object using ArRobot::findLaser().
	myRobot->lock();
	std::map<int, ArLaser*> *lasers = myRobot->getLaserMap();

	//ArLog::log(ArLog::Normal, "ArRobot provided a set of %d ArLaser objects.", lasers->size());

	for (std::map<int, ArLaser*>::const_iterator i = lasers->begin(); i != lasers->end(); ++i)
	{
		int laserIndex = (*i).first;
		ArLaser* laser = (*i).second;
		if (!laser)
			continue;
		++numLasers;
		laser->lockDevice();

		// The current readings are a set of obstacle readings (with X,Y positions as well as other attributes) that are the most recent set from teh laser.
		std::list<ArPoseWithTime*> *currentReadings = laser->getCurrentBuffer(); // see ArRangeDevice interface doc

		// The raw readings are just range or other data supplied by the sensor. It may also include some device-specific extra values associated with each reading as well. (e.g. Reflectance for LMS200)
		//		const std::list<ArSensorReading*> *rawReadings = laser->getRawReadings();

		ArLog::log(ArLog::Normal, "Laser #%d (%s): %s.\n\tHave %d 'current' readings.\n\t", //Have %d 'raw' readings.\n\t",
			laserIndex, laser->getName(), (laser->isConnected() ? "connected" : "NOT CONNECTED"),
			currentReadings->size());
			
		//			rawReadings->size())
		bool done = false;		

		laser->unlockDevice();
	}
	if (numLasers == 0)
		ArLog::log(ArLog::Normal, "No lasers.");
	else
		ArLog::log(ArLog::Normal, "");

	// Unlock robot and sleep for 5 seconds before next loop.
	myRobot->unlock();
}
