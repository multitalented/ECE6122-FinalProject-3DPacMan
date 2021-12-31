/*
Author: Deanna Gierzak
Class: ECE 6122
Last Date Modified: 10/28/2020

Description:

This MazeLevel class handles the bulk of the creation of the level in which
Pac-man lives.

It defines the maze level space as a 19 x 22 grid, with the coordinate
system as follows, ( o the approximate location of the camera)

Y
|
|_______________
|				|
|    (maze)		|
|				|
|_______________|________> X
(0,0)


o

The level is built from an input .txt file containing values of
W, D, P, and O which mean wall, pac-dots, power-ups, and nothing
respectively. Their location in the txt file corresponds to their
location in the array which is translated to the coordinate system
above. From this file, boolean arrays are created for each of the maze
objects to be created. These then determine where to place
walls, pac-dots, and power-ups.

Example of how an input .txt file should look and how it relates to the
coordinate system defined above

|----------> x
|
|	WWWWWWWWWWWWWWWWWWW
|	WDDDDDDDDDDDDDDDDDW
|	WDWWWWWWDWDWWWWWWDW
|	WDDDDWDDDWDDDWDDDDW
y	WWDWDWDWWWWWDWDWDWW
	WPDWDDDDDODDDDDWDPW
	WDWWDWWWDWDWWWDWWDW
	WDDDDDDDDWDDDDDDDDW
	WWWWDWOWWWWWOWDWWWW
	OOOWDWOOOOOOOWDWOOO
	WWWWDWOWWWWWOWDWWWW
	OOOODOOWOOOWOODOOOO
	WWWWDWOWWWWWOWDWWWW
	OOOWDWOOOOOOOWDWOOO
	WWWWDWWWOWOWWWDWWWW
	WDDDDWDDDWDDDWDDDDW
	WDWWDWDWWWWWDWDWWDW
	WDDDDDDDDDDDDDDDDDW
	WDWWDWWWDWDWWWDWWDW
	WPWWDWWWDWDWWWDWWPW
	WDDDDDDDDWDDDDDDDDW
	WWWWWWWWWWWWWWWWWWW

See MazeLevel.cpp for detailed descriptions of each function.

*/

#pragma once

//#include <GL/glew.h> // Include the GLEW header file
//#include <conio.h>
#include <iostream>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // standard definitions
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <vector>


//enum mazeType {	wall, coin, power };


using namespace std;

class MazeLevel
{
private:
	//float location;						// holds the location 

public:
	char levelArray[22][19];			// holds the values taken in from the text file (W for wall, D for pac-dots, P for power-ups)
	int iLevelArray[22][19];

	int xWallsArray[22][19];			// holds the 0/1 values used to create the walls parallel to the x-axis
	int yWallsArray[22][19];			// holds the 0/1 values used to create the walls parallel to the y-axis

	int wallsArray[22][19];				// holds the 0/1 values used to determine where the walls are located
	int wallCornersArray[22][19];		// holds the 0/1 values used to determine where wall corners are to place spheres for rounded corners
	int pacDotsArray[22][19];			// holds the 0/1 values used to determine where the pac-dots are located
	int powerUpsArray[22][19];			// holds the 0/1 values used to determine where the power-ups are located

//	mazeType mazeArray[22][19];

	double wallCylinderRadius = 1.0;				// radius of the cylinders used for creating the walls
	float pacDotRadius = 1.5;					// radius of the spheres used for creating the pac-dots
	double powerUpRadius = 3.0;					// radius of the cylinders used for creating the power-ups

	long score;							// holds the player's score

	void readFile(const string& filename);		// function to read in the text file of W, D, P values to create the maze

	void clearLevel();					// sets all of the object's array values to 0
	void render();						// render the scene (walls, pac-dots, power-ups)
	void renderChanges();				// render only the changes as they get updated (pac-dots and power-ups)

	void renderPacDots();				// renders the pac-dots from its boolean array
	void renderPowerUps();				// renders the power-ups from its boolean array
	void renderMazeWalls();				// renders the maze walls from its boolean array
	void renderMazeWallCorners();		// renders the maze wall corners from its boolean array




	float colorBlue[4] = { 0.0f, 0.1f, 0.9f, 1.0f };
	float ambBlue[4] = { 0.1f, 0.2f, 0.9f, 1.0f };
	float diffBlue[4] = { 0.1f, 0.2f, 0.9f, 1.0f };
	float specBlue[4] = { 0.05f, 0.05f, 0.05f, 1.0f };

	float colorWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float ambWhite[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float diffWhite[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float specWhite[4] = { 0.03f, 0.03f, 0.03f, 1.0f };


};