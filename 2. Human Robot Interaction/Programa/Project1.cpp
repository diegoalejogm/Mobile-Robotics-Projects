#include "Aria.h"
#include<cmath>
#include <iostream>
#include <string.h>
#include "ActionGo.h"

using namespace Leap;

class SampleListener : public Listener {
public:
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
};

void SampleListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);
}


void SampleListener::onFrame(const Controller& controller) {
//	std::cout << "Frame available" << std::endl;
	const Frame frame = controller.frame();
	Leap::Hand leftmostHand = frame.hands().leftmost();
	Leap::Hand rightmostHand = frame.hands().rightmost();
	
	int i = 0;
	if (leftmostHand.grabStrength() + rightmostHand.grabStrength() == 2)
	{
		std::cout << "Ambas manos cerradas" + ++i << std::endl;
	}
}


int main(int argc, char** argv)
{
	Aria::init();
	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();
	ArRobot robot;
	ArSonarDevice sonar;


	ArRobotConnector robotConnector(&parser, &robot);
	ArLaserConnector laserConnector(&parser, &robot, &robotConnector);

	if (!robotConnector.connectRobot())
	{
		ArLog::log(ArLog::Terse, "robotSyncTaskExample: Could not connect to the robot.");
		if (parser.checkHelpAndWarnUnparsed())
		{
			// -help not given
			Aria::logOptions();
			Aria::exit(1);
		}
	}

	if (!Aria::parseArgs() || !parser.checkHelpAndWarnUnparsed())
	{
		Aria::logOptions();
		Aria::exit(1);
	}

	// Connect to laser(s) defined in parameter files, if LaseAutoConnect is true
	// (Some flags are available as arguments to connectLasers() to control error behavior and to control which lasers are put in the list of lasers stored by ArRobot. See docs for details.)
	if (!laserConnector.connectLasers())
	{
		ArLog::log(ArLog::Terse, "Could not connect to configured lasers. Exiting.");
		Aria::exit(3);
		return 3;
	}

	ArLog::log(ArLog::Normal, "Connected to all lasers.");

	ArLog::log(ArLog::Normal, "robotSyncTaskExample: Connected to robot.");

	// This object encapsulates the task we want to do every cycle. 
	// Upon creation, it puts a callback functor in the ArRobot object
	// as a 'user task'.
	//PrintingTask pt(&robot);
	//LeapControlTask sdtt(&robot);

	robot.addRangeDevice(&sonar);

	// the keydrive action
	//ArActionKeydrive keydriveAct;
	ActionGo go(500,350);
	// limiter for close obstacles
	ArActionLimiterForwards limiter("speed limiter near", 60, 200, 250);
	// the keydrive action
	ArActionKeydrive keydriveAct;
	// limiter so we don't bump things backwards
	ArActionLimiterBackwards backwardsLimiter;

	robot.enableMotors();
	robot.addAction(&go, 50);
	robot.addAction(&keydriveAct, 45);
	
	robot.addAction(&limiter, 95);
	//robot.addAction(&limiterFar, 90);

	// Start the robot process cycle running. Each cycle, it calls the robot's
	// tasks. When the PrintingTask was created above, it added a new
	// task to the robot. 'true' means that if the robot connection
	// is lost, then ArRobot's processing cycle ends and this call returns.
	robot.run(true);


	printf("Disconnected. Goodbye.\n");
	

	// ------------ LEAP MOTION ---------------
	/*
	SampleListener listener;
	Controller controller;

	controller.addListener(listener);

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit..." << std::endl;
	std::cin.get();

	// Remove the sample listener when done
	controller.removeListener(listener);
	*/

	return 0;
}
