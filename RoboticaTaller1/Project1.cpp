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
	ArLog::log(ArLog::Normal, "\r %d", myRobot->lock());

	printf("\rx %6.1f  y %6.1f  th  %6.1f vel %7.1f mpacs %3d", myRobot->getX(),
		myRobot->getY(), myRobot->getTh(), myRobot->getVel(),
		myRobot->getMotorPacCount());

	myRobot->unlock();

	// Need sensor readings? Try myRobot->getRangeDevices() to get all 
	// range devices, then for each device in the list, call lockDevice(), 
	// getCurrentBuffer() to get a list of recent sensor reading positions, then
	// unlockDevice().
}

int main(int argc, char** argv)
{
	Aria::init();
	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();
	ArRobot robot;

	ArRobotConnector robotConnector(&parser, &robot);
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

	ArLog::log(ArLog::Normal, "robotSyncTaskExample: Connected to robot.");

	// This object encapsulates the task we want to do every cycle. 
	// Upon creation, it puts a callback functor in the ArRobot object
	// as a 'user task'.
	PrintingTask pt(&robot);

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

	printf("Disconnected. Goodbye.\n");

	return 0;
}
