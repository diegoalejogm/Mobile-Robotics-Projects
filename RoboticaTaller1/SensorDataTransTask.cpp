#include "SensorDataTransTask.h"
#include "Transformation.h"
#include "Aria.h"
#include <list>
#include <iostream>
#define PI acos(-1.0)

// the constructor (note how it uses chaining to initialize myTaskCB)
SensorDataTransTask::SensorDataTransTask(ArRobot *robot) :
myTaskCB(this, &SensorDataTransTask::doTask)
{
	myRobot = robot;
	outputFile = fopen("data.map", "w");
	fprintf(outputFile, "2D-Map\nDATA\n");
	// just add it to the robot
	myRobot->addSensorInterpTask("DataTransformationTask", 50, &myTaskCB);
}

SensorDataTransTask::~SensorDataTransTask()
{
	myRobot->remSensorInterpTask(&myTaskCB);
}

void SensorDataTransTask::doTask(void)
{
	// We will lock the robot while using it to prevent changes by tasks in the robot's background task thread or any other threads. Each laser has an index. You can also store the laser's index or name (laser->getName()) and use that to get a reference (pointer) to the laser object using ArRobot::findLaser().
	myRobot->lock();

	Transformation currentPos = Transformation(Point2D(myRobot->getX(), myRobot->getY()), myRobot->getTh()*PI / 180);
	bool hasChanged = !(currentPos == lastKnownLocation);
	lastKnownLocation = currentPos;
	
	int numLasers = 0;

	// Get a pointer to ArRobot's list of connected lasers.
	std::map<int, ArLaser*> *lasers = myRobot->getLaserMap();

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
		for (std::list<ArPoseWithTime*>::iterator it = currentReadings->begin(); it != currentReadings->end() && laser->isConnected() && hasChanged; it++) {
			IntegerPoint2D point = IntegerPoint2D((int)floor((*it)->getX()), (int)floor((*it)->getY()));
			if (knownPoints.find(point) == knownPoints.end()) {
				knownPoints.insert(point);
				fprintf(outputFile, "%d\t%d\n", point.x, point.y);
			}
		}
		laser->unlockDevice();
	}

	if (numLasers == 0)
		ArLog::log(ArLog::Normal, "No lasers.");
	else
		ArLog::log(ArLog::Normal, "");
	ArLog::log(ArLog::Normal, "Points Detected: %d", knownPoints.size());
	// Unlock robot and sleep for 5 seconds before next loop.
	myRobot->unlock();
	//ArUtil::sleep(5000);
}