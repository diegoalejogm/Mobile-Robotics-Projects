#include "SensorDataTask.h"
#include "Transformation.h"
#include "Aria.h"
#include <list>
#include <iostream>
#include <fstream>

#define PI acos(-1.0)
#define EPS 1e-6
#define POS_FILENAME ""

// the constructor (note how it uses chaining to initialize myTaskCB)
SensorDataTask::SensorDataTask(ArRobot *robot) :
myTaskCB(this, &SensorDataTask::doTask)
{
	myRobot = robot;

	std::ifstream inFile("position.txt");
	
	float x, y, th;
	inFile >> x >> y >> th;
	
	ArLog::log(ArLog::Normal, "x %f  y %f  th  %f", x, y, th);
	sceneTransformation = Transformation(Point2D(x, y), th*PI / 180);

	robotFrameOutputFile = fopen("robotFrame.map", "w");
	sceneFrameOutputFile = fopen("sceneFrame.map", "w");

	fprintf(robotFrameOutputFile, "2D-Map\nDATA\n");
	fprintf(sceneFrameOutputFile, "2D-Map\nDATA\n");
	// just add it to the robot
	myRobot->addSensorInterpTask("SensorDataTask", 50, &myTaskCB);
}

SensorDataTask::~SensorDataTask()
{
	myRobot->remSensorInterpTask(&myTaskCB);
}

void SensorDataTask::doTask(void)
{
	// We will lock the robot while using it to prevent changes by tasks in the robot's background task thread or any other threads. Each laser has an index. You can also store the laser's index or name (laser->getName()) and use that to get a reference (pointer) to the laser object using ArRobot::findLaser().
	myRobot->lock();

	Transformation currentPos = Transformation(Point2D(myRobot->getX(), myRobot->getY()), myRobot->getTh()*PI / 180);
	bool hasChanged = false;
	if (currentPos.translation.dist(lastKnownLocation.translation) > 3.0 - EPS || abs(currentPos.rotation - lastKnownLocation.rotation)>PI/45) {
		lastKnownLocation = currentPos;
		hasChanged = true;
	}
	
	
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
			Point2D rFramePoint = Point2D((*it)->getX(), (*it)->getY());
			IntegerPoint2D rFrameIntPoint = IntegerPoint2D(rFramePoint);
			if (knownPoints.find(rFrameIntPoint) == knownPoints.end()) {
				knownPoints.insert(rFrameIntPoint);
				fprintf(robotFrameOutputFile, "%d\t%d\n", rFrameIntPoint.x, rFrameIntPoint.y);
				Point2D sceneFramePoint = sceneTransformation.apply(rFramePoint);
				IntegerPoint2D sceneFrameIntPoint = IntegerPoint2D(sceneFramePoint);
				fprintf(sceneFrameOutputFile, "%d\t%d\n", sceneFrameIntPoint.x, sceneFrameIntPoint.y);
			}
		}
		laser->unlockDevice();
	}

	if (numLasers == 0)
		ArLog::log(ArLog::Normal, "No lasers.");
	else
		ArLog::log(ArLog::Normal, "");
	//ArLog::log(ArLog::Normal, "Points Detected: %d", knownPoints.size());
	// Unlock robot and sleep for 5 seconds before next loop.
	myRobot->unlock();
	//ArUtil::sleep(5000);
}