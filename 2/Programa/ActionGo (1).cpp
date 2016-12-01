#include "ActionGo.h"
#include "Leap.h"
#include <iostream>
#include <string.h>

using namespace Leap;

/*
Note the use of constructor chaining with
ArAction(actionName). Also note how it uses setNextArgument, which makes it so that
other parts of the program could find out what parameters this action has, and possibly modify them.
*/
ActionGo::ActionGo(double maxSpeed, double stopDistance) :
ArAction("Go")
{
	mySonar = NULL;
	leapController = Leap::Controller();
	leapController.enableGesture(Leap::Gesture::TYPE_CIRCLE);

}

/*
Override ArAction::setRobot() to get the sonar device from the robot, or deactivate this action if it is missing.
You must also call ArAction::setRobot() to properly store
the ArRobot pointer in the ArAction base class.
*/
void ActionGo::setRobot(ArRobot *robot)
{
	ArAction::setRobot(robot);
	mySonar = robot->findRangeDevice("sonar");
	if (robot == NULL)
	{
		ArLog::log(ArLog::Terse, "actionExample: ActionGo: Warning: I found no sonar, deactivating.");
		deactivate();
	}
}

/*
This fire is the whole point of the action.
currentDesired is the combined desired action from other actions
previously processed by the action resolver.  In this case, we're
not interested in that, we will set our desired
forward velocity in the myDesired member, and return it.

Note that myDesired must be a class member, since this method
will return a pointer to myDesired to the caller. If we had
declared the desired action as a local variable in this method,
the pointer we returned would be invalid after this method
returned.
*/
ArActionDesired *ActionGo::fire(ArActionDesired currentDesired)
{

	// reset the actionDesired (must be done), to clear
	// its previous values.
	myDesired.reset();

	// if the sonar is null we can't do anything, so deactivate
	if (mySonar == NULL)
	{
		deactivate();
		return NULL;
	}

	
	Leap::Frame frame = leapController.frame(); //The latest frame
	Leap::HandList handsInFrame = frame.hands();
	Leap::GestureList gestures = frame.gestures();

	if (handsInFrame.count() == 1)
	{
		Leap::Hand hand = handsInFrame[0];

		if (hand.grabStrength() <= 0.15) myDesired.setVel(mySpeed);
		else if (hand.grabStrength() >= 0.85) myDesired.setVel(0);
		else if (hand.palmNormal().roll() > -Leap::PI && hand.palmNormal().roll() < -Leap::PI/2)

	}
	
	/*

	
	for (Leap::GestureList::const_iterator gl = gestures.begin(); gl != gestures.end(); gl++){

		Leap::Gesture gesture = (*gl);
		if (gesture.type() == Leap::Gesture::TYPE_CIRCLE){
			Leap::CircleGesture circleGesture = Leap::CircleGesture(gesture);

			if (circleGesture.pointable().direction().angleTo(circleGesture.normal()) <= Leap::PI / 2) {
				//clockwiseness = "clockwise";
				mySpeed += 10;
			}
			else
			{
				//clockwiseness = "counterclockwise";
				mySpeed -= 10;
			}
		}
	}

	

	if (handsInFrame.count > 0)
	{
		bool leftHandPinch = false, rightHandPinch = false;

		for (Leap::HandList::const_iterator hl = handsInFrame.begin(); hl != handsInFrame.end(); hl++)
		{
			Leap::Hand hand = (*hl);
			if (hand.grabStrength() > 0.9 && hand.isLeft())
			{
				leftHandPinch = true;
			}
			if (hand.grabStrength() > 0.9 && hand.isRight())
			{
				rightHandPinch = true;
			}
		}
		if (leftHandPinch && !rightHandPinch) { myDesired }
	}

	// Speed forward
	if (handsInFrame.count() == 2){
		
		bool closed = true;
		for (Leap::HandList::const_iterator hl = handsInFrame.begin(); hl != handsInFrame.end(); hl++)
		{
			closed &= (*hl).grabStrength() >= 0.75;
		}
		if (closed) myDesired.setVel(mySpeed);
		else myDesired.setVel(0);
	
	else
	{
		myDesired.setVel(0);
	}

	if (mySpeed < 0) mySpeed = 0;
	if (mySpeed > 400) mySpeed = 400;

	
	std::cout << mySpeed << std::endl;


	*/
	// return a pointer to the actionDesired to the resolver to make our request
	return &myDesired;
}

