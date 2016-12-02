#include "Aria.h"

int main(int argc, char **argv)
{
	Aria::init();
	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();
	ArRobot robot;
	ArAnalogGyro gyro(&robot);
	ArSonarDevice sonar;
	ArRobotConnector robotConnector(&parser, &robot);
	ArLaserConnector laserConnector(&parser, &robot, &robotConnector);


	// Connect to the robot, get some initial data from it such as type and name,
	// and then load parameter files for this robot.
	if (!robotConnector.connectRobot())
	{
		ArLog::log(ArLog::Terse, "gotoActionExample: Could not connect to the robot.");
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

	ArLog::log(ArLog::Normal, "gotoActionExample: Connected to robot.");

	robot.addRangeDevice(&sonar);
	robot.runAsync(true);

	// Make a key handler, so that escape will shut down the program
	// cleanly
	ArKeyHandler keyHandler;
	Aria::setKeyHandler(&keyHandler);
	robot.attachKeyHandler(&keyHandler);
	printf("You may press escape to exit\n");

	// Collision avoidance actions at higher priority
	ArActionLimiterForwards limiterAction("speed limiter near", 300, 600, 250);
	ArActionLimiterForwards limiterFarAction("speed limiter far", 300, 1100, 400);
	ArActionLimiterTableSensor tableLimiterAction;
	robot.addAction(&tableLimiterAction, 100);
	robot.addAction(&limiterAction, 95);
	robot.addAction(&limiterFarAction, 90);

	// Goto action at lower priority
	ArActionGoto gotoPoseAction("goto",ArPose(0.0,0.0,0.0),100.0,400,250,7);
	robot.addAction(&gotoPoseAction, 50);

	// Stop action at lower priority, so the robot stops if it has no goal
//	ArActionStop stopAction("stop");
//	robot.addAction(&stopAction, 40);


	// turn on the motors, turn off amigobot sounds
	robot.enableMotors();
	robot.comInt(ArCommands::SOUNDTOG, 0);

	
	const int duration = 30000; //msec
	ArLog::log(ArLog::Normal, "Going to four goals in turn for %d seconds, then cancelling goal and exiting.", duration / 1000);




	bool first = true;
	int goalNum = 0;
	ArTime start;
	start.setToNow();
	while (Aria::getRunning())
	{
		robot.lock();

		// Choose a new goal if this is the first loop iteration, or if we 
		// achieved the previous goal.
		if (first)
		{
			first = false;
			gotoPoseAction.setGoal(ArPose(2500, 0));
			
			ArLog::log(ArLog::Normal, "Going to next goal at %.0f %.0f",
				gotoPoseAction.getGoal().getX(), gotoPoseAction.getGoal().getY());
		}
		else if (gotoPoseAction.haveAchievedGoal())
		{
			break;
		}

		robot.unlock();
		ArUtil::sleep(100);
	}

	// Robot disconnected or time elapsed, shut down
	Aria::exit(0);
	return 0;
}