#include "Configuration.h"
#include "PathPlanning.h"
#include "Aria.h"

#include <iostream>
#include <ctime>
using namespace std;
int orientation(int from, int to) {
	if (to < 0) to += 360;
	if (from < 0) from += 360;
	if (to - from >= 180 || (to - from < 0) && (to-from > -180) ) return -1;
	return 1;
}
bool close(int anxg1, double ang2) {
	double ang1 = (double) anxg1;
	if (ang1 > ang2) swap(ang1, ang2);
	double dist = min(abs(ang2 - ang1), abs(ang2 - ang1 + 360));
//	cout << dist << endl;
	if (dist < 2.5) return true;
	return false;
}
int main(int argc, char** argv)
{
	clock_t t1, t2;
	t1 = clock();
	PathPlanning::Init (); 
	t1 = clock() - t1;
	cout << "Initial planning: " << ((double) t1) / CLOCKS_PER_SEC << endl;
	// initialize ARIA
	Aria::init();

	// used to parse command-line arguments. only one instance needed.
	ArArgumentParser argParser(&argc, argv);

	// get hostnames and port numbers for connecting to the robots.
	const char* host1 = argParser.checkParameterArgument("-rh1");
	if (!host1) host1 = "localhost";
	const char* host2 = argParser.checkParameterArgument("-rh2");
	if (!host2) host2 = "localhost";
	int port1 = 8101;
	int port2 = 8101;
	if (strcmp(host1, host2) == 0)
	{
		// same host, it must be using two ports (but can still override below with -rp2)
		port2++;
	}

	bool argSet = false;
	argParser.checkParameterArgumentInteger("-rp1", &port1, &argSet);
	if (!argSet) argParser.checkParameterArgumentInteger("-rrtp1", &port1);
	argSet = false;
	argParser.checkParameterArgumentInteger("-rp2", &port2, &argSet);
	if (!argSet) argParser.checkParameterArgumentInteger("-rrtp2", &port2);

	if (!argParser.checkHelpAndWarnUnparsed())
	{
		ArLog::log(ArLog::Terse, "Usage: twoRobotWander [-rh1 <hostname1>] [-rh2 <hostname2>] [-rp1 <port1>] [-rp2 <port2>]\n"\
			"\t<hostname1> Is the network host name of the first robot."\
			" Default is localhost (for the simulator).\n"\
			"\t<hostname2> Is the network host name of the second robot."\
			" Default is localhost (for the simulator).\n"\
			"\t<port1> Is the TCP port number of the first robot. Default is 8101.\n"\
			"\t<port2> Is the TCP port number of the second robot. Default is 8102 if"\
			" both robots have the same hostname, or 8101 if they differ.\n\n");
		return 1;
	}



	//
	// The first robot's objects
	//
	// the first robot connection
	ArTcpConnection con1;
	// the first robot interface
	ArRobot robot1;
	// sonar interface
	ArSonarDevice sonar1;
	// the actions we'll use to cause wander behavior for the first robot
	ArActionGoto gotoPoseAction1("goto", ArPose(0.0, 0.0, 0.0), 100.0, 300, 150, 7);
	ArActionStop stopAction1("stop");

	//
	// The second robot's objects
	//
	// the second robot connection
	ArTcpConnection con2;
	// the second robot
	ArRobot robot2;
	// sonar
	ArSonarDevice sonar2;
	// the actions we'll use to wander for the second robot
	ArActionGoto gotoPoseAction2("goto", ArPose(0.0, -1.0, 0.0), 100.0, 300, 150, 7);
	ArActionStop stopAction2("stop");
	

	//
	// Lets get robot 1 going
	//
	int ret;
	std::string str;

	// open the connection, if this fails exit
	ArLog::log(ArLog::Normal, "Connecting to first robot at %s...", host1);
	if ((ret = con1.open(host1,port1)) != 0)
	{
		str = con1.getOpenMessage(ret);
		printf("Open failed to robot 1: %s\n", str.c_str());
		Aria::exit(1);
		return 1;
	}

	// add the sonar to the robot
	robot1.addRangeDevice(&sonar1);

	// set the device connection on the robot
	robot1.setDeviceConnection(&con1);

	// try to connect, if we fail exit
	if (!robot1.blockingConnect())
	{
		printf("Could not connect to robot 1... exiting\n");
		Aria::exit(1);
		return 1;
	}

	// turn on the motors, turn off amigobot sounds
	robot1.comInt(ArCommands::ENABLE, 1);
	robot1.comInt(ArCommands::SOUNDTOG, 0);

	// add the actions
	robot1.addAction(&gotoPoseAction1, 50);
	robot1.addAction(&stopAction1, 40);



	//
	// Lets get robot 2 going
	//

	// open the connection, if this fails exit
	//ArLog::log(ArLog::Normal, "Connecting to second robot at %s:%d...", host2, port2);
	if ((ret = con2.open(host2,port2)) != 0)
	{
		str = con2.getOpenMessage(ret);
		printf("Open failed to robot 2: %s\n", str.c_str());
		Aria::exit(1);
		return 1;
	}

	// add the sonar to the robot
	robot2.addRangeDevice(&sonar2);

	// set the device connection on the robot
	robot2.setDeviceConnection(&con2);

	// try to connect, if we fail exit
	if (!robot2.blockingConnect())
	{
		printf("Could not connect to robot 2... exiting\n");
		Aria::exit(1);
		return 1;
	}

	// turn on the motors, turn off amigobot sounds
	robot2.comInt(ArCommands::ENABLE, 1);
	robot2.comInt(ArCommands::SOUNDTOG, 0);

	// add the actions
	robot2.addAction(&gotoPoseAction2, 50);
	robot2.addAction(&stopAction2, 40);

	// start the robots running. true so that if we lose connection to either
	// robot, the run stops.
	robot1.runAsync(true);
	robot2.runAsync(true);

	// As long as both robot loops are running, lets alternate between the
	// two wandering around every 10 sec.  mutex lock/unlock calls are neccesary since 
	// the ArRobot objects are running in background threads after runAsync()
	// calls above.
	bool first = true;
	bool last = false;

	bool done1 = true;
	bool done2 = true;

	ArPose pose1 = ArPose(0, 0, 0);
	ArPose pose2 = ArPose(0, 0, 0);
	vector<Configuration> robot1Path = PathPlanning::getRobotPath(1);
	vector<Configuration> robot2Path = PathPlanning::getRobotPath(2);
	int currentStep = 0;
	bool rotating1 = false;
	bool rotating2 = false;
	int currentAngle1 = 0, currentAngle2 = 0;
	t2 = clock();
	
	while (robot1.isRunning() && robot2.isRunning() && currentStep <= robot1Path.size())
	{
		ArUtil::sleep(100);
		robot1.lock();
		robot2.lock();
		//cout << done1 << " " << done2 << " " << currentStep << " "<< rotating1<< " "<< rotating2 <<endl;
		if (rotating1 && close(currentAngle1, robot1.getPose().getTh())) {
			
			robot1.setRotVel(0);
			done1 = true;
			rotating1 = false;
		}
		if (!done1 && !rotating1 && gotoPoseAction1.haveAchievedGoal()) {
			done1 = true;
		}
		if (rotating2 && close(currentAngle2, robot2.getPose().getTh())) {
			robot2.setRotVel(0);
			done2 = true;
			rotating2 = false;
		}
		if (!done2 && !rotating2 && gotoPoseAction2.haveAchievedGoal()) {
			done2 = true;
		}
		if (done1 && done2) {

			done1 = false;
			done2 = false;
			if (currentStep < robot1Path.size()) {
				pose1 = ArPose(robot1Path[currentStep].position.x, robot1Path[currentStep].position.y, robot1Path[currentStep].angle);
				pose2 = ArPose(robot2Path[currentStep].position.x, robot2Path[currentStep].position.y, robot2Path[currentStep].angle);
				if (currentAngle1 != robot1Path[currentStep].angle) {
					rotating1 = true;
					robot1.setRotVel(10 * orientation(currentAngle1, robot1Path[currentStep].angle));
					currentAngle1 = robot1Path[currentStep].angle;
				}
				else {
					gotoPoseAction1.setGoal(pose1);
				}
				if (currentAngle2 != robot2Path[currentStep].angle) {
					rotating2 = true;
					robot2.setRotVel(10 * orientation(currentAngle2, robot2Path[currentStep].angle));
					currentAngle2 = robot2Path[currentStep].angle;
				}
				else {
					gotoPoseAction2.setGoal(pose2);
				}
			}
			currentStep++;
		}
		/*
		if (first)
		{
			first = false;
			gotoPoseAction1.setGoal(ArPose(0, 0, 0.0));
			gotoPoseAction2.setGoal(ArPose(1500, -1, 0.0));
		}
		else if (!last && gotoPoseAction1.haveAchievedGoal() && gotoPoseAction2.haveAchievedGoal())
		{
			last = true;
			gotoPoseAction1.setGoal(ArPose(2500, 0, 0.0));
			gotoPoseAction2.setGoal(ArPose(1550, -1, 180.0));
		}
		else if (last && gotoPoseAction1.haveAchievedGoal() && gotoPoseAction2.haveAchievedGoal())
		{
			break;
		}
		*/
		robot2.unlock();
		robot1.unlock();
	}

	t2 = clock() - t2;
	cout << "Execution time " << ((double)t2) / CLOCKS_PER_SEC << endl;

	// exit program if both robots disconnect.
	ArLog::log(ArLog::Normal, "Both robots disconnected.");
	Aria::exit(0);
	return 0;
}

/*

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
*/