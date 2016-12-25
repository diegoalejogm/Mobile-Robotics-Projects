# Project \#1: Robot manipulation and surroundings recognition

This project consisted in creating a program capable of manipulating a robot through the keyboard. Additionally, the robot was capable of creating a map of the explored scenario by capturing proximity information from its surroundings.

This application can be either downloaded and executed, or modified and and compiled. Follow the _Install_ instructions if you want to modify it, and the _Usage_ instructions in order to use the (already existing) compiled project.

## Install

This project requires MobileRobots' [Advanced Robot Interface for Applications (ARIA)](http://robots.mobilerobots.com/wiki/ARIA) 2.9.1 installed. Documentation on how to configure it in Visual Studio can be found [here](http://robots.mobilerobots.com/ARIA/download/current/README.txt). 

## Usage

Make sure to place a file _position.txt_ under the _Release_ Folder with the initial position of the robot (x,y,theta), relative to the scene. The correct formatting can be found in the present sample.

The robot controlled with this application can either be simulated or real. 
* If a physical robot is used, make sure to execute the application through the command line and specify the robot's ip address.
* If executed locally, run the application through the console or GUI.

### Instructions
1. Inside _Release_ folder resides the file _RoboticaTaller1.exe_, which is the main application. Execute it in order to start the system.
2. During execution the user can control the robot with the keyboard keys. They allow to change the robot's speed and angle. 
3. To finish the simulation, press the **_ESC_** keyboard key.
4. After terminating execution, two files will be generated in the _Release_ folder:
⋅⋅*_robotFrame.map_: This file contains the map of the detected obstacles by the robot's sensor, relative to the initial robot position.
⋅⋅*_sceneFrame.map_: This file contains the map of the detected obstacles by the robot's sensor, relative to the scene's frame of reference.

## Credits

* Diego Gomez
* Rafael Mantilla

# License

MIT
