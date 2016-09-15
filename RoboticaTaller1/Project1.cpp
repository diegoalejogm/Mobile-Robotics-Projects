#include "Aria.h"
#include "PrintingTask.h"
#include "SensorDataTransTask.h"
#include<cmath>

int main(int argc, char** argv)
{

	Aria::init();
	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();
	ArRobot robot;

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
	// for the laser. 
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
	PrintingTask pt(&robot);
	SensorDataTransTask sdtt(&robot);

	// initialize aria
	Aria::init();

	// the keydrive action
	ArActionKeydrive keydriveAct;

	robot.enableMotors();
	robot.addAction(&keydriveAct, 45);

	// Start the robot process cycle running. Each cycle, it calls the robot's
	// tasks. When the PrintingTask was created above, it added a new
	// task to the robot. 'true' means that if the robot connection
	// is lost, then ArRobot's processing cycle ends and this call returns.
	robot.run(true);

	// Allow some time to read laser data
	ArUtil::sleep(500);

	printf("Disconnected. Goodbye.\n");

	return 0;
}
