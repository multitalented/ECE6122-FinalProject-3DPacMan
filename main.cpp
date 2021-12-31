/*
Author: Deanna Gierzak
Class: ECE 6122
Last Date Modified: 12/01/2020

Description:

This software is a Pac-Man game built with the OpenGL graphics library,
that currently allows clockwise rotation of the maze with user input
of 'R' and Pac-Man movement with the arrow keys.

The game relies on classes ECE_Pacman to create pac-man, ECE_Ghost to create
the four ghosts, and MazeLevel to create, render, and store relevant values
for the maze level and interaction with pac-man in gameplay

The Pac-man game maze level space is a 190 x 220 grid, with the coordinate
system as follows (or 19 x 22 defined in MazeLevel class arrays)

Y
|
|_______________
|				|
|    (maze)		|
|				|
|_______________|________> X
(0,0)



			o   <--- initial camera location, approx.


	Ghosts run in their own thread, chasing after pac-man by a vector path when normal,
		and moving randomly when made vulnerable by pac-man consumption of power-up.
		The ghosts are released in 2 second intervals.
	The game is over after pac-man has eaten all the pac-dots and power-ups in the
		maze or when the player has used all 3 lives (therefore game is over when player
		dies at 0 lives). 
	When pac-man eats a power-up, he can eat ghosts for 5 seconds.

	All final project requirements have been satisfied to the best of my knowledge
		and confirmed via debugging on my local machine.

		(Only exceptions are that the ghosts do not use an actual shortest path
		algorithm - I created one for them to use that goes after pac-man by
		shortest x or y distance. Additionally, when pac-man is powered up,
		the ghosts do not move directly away from pac-man, but instead are coded
		to move in a random direction)


*/


//#include <GL/glew.h> // Include the GLEW header file
//#include <conio.h>
#include <iostream>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // standard definitions
#include <fstream>
#include <string>
#include <vector>

// for multithreading and timing, final project
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <atomic>

#include <iomanip>

#include <map>

#include "ECE_Pacman.h"
#include "ECE_Ghost.h"
#include "MazeLevel.h"
#include "ECE_Bitmap.h"

#define ESC 27

GLUquadricObj *quadratic = gluNewQuadric();

// lighting position
GLfloat light_position[] = { -300.0f, -200.0f, 600.0f, 0.0f }; //{ 200.0f, 200.0f, 200.0f, 0.0f };

//----------------------------------------------------------------------
// Global variables
//
// The coordinate system is set up so that the (x,y)-coordinate plane
// is the ground, and the z-axis is directed upwards. The y-axis points
// to the north and the x-axis points to the east.
//
// The values (x,y) are the current camera position. The values (lx, ly)
// point in the direction the camera is looking. The variables angle and
// deltaAngle control the camera's angle. The variable deltaMove
// indicates the amount of incremental motion for the camera with each
// redraw cycle. The variables isDragging and xDragStart are used to
// monitor the mouse when it drags (with the left button down).
//----------------------------------------------------------------------

// Camera position & rotation
//float x = 0.0, y = -5.0; // initially 5 units south of origin
float pi = 3.14159265358979323846;
//float pi = 3.14159265358979;

// Maze, Camera, and Rotation parameters:
// Values are defined such that rotation could be done by rotation the scene or the moving the camera
//		Currently the camera is set for rotation, and the the code that would rotate the scene is commented out
double rotation = 5 * pi / 180;
double cameraX = 500, cameraY = -500, cameraZ = 600;	// desired camera starting location (x and y values are used to calculate the radius
double eyeX = cameraX, eyeY = cameraY;
double mazeX = 180.0, mazeY = 210.0, mazeZ = 0;
double centerX = mazeX / 2, centerY = mazeY / 2, centerZ = 0;
float mazeRotate = 0;

// the desired initial position for the camera with the parameterized formula works out to an i = 61
long i = 61;
// the radius from the center of the maze to the camera's desired starting location along a circle projected onto the x-y plane
const double RADIUS = sqrt(pow((cameraX - centerX), 2) + pow((cameraY - centerY), 2));

//  Pac-Man
ECE_Pacman player;

// Ghosts (4 of them)
ECE_Ghost rGhost(red);
ECE_Ghost pGhost(pink);
ECE_Ghost oGhost(orange);
ECE_Ghost gGhost(green);

//ECE_Ghost ghosts[4]; 
// why doesn't array work either???
/*
ghosts[0] = rGhost;
ghosts[1] = pGhost;
ghosts[2] = oGhost;
ghosts[3] = gGhost;*/
//{ rGhost, pGhost, oGhost, gGhost };
//vector<ECE_Ghost> allGhosts;
//allGhosts.push_back(rGhost);
//vector<int> vecRow2{ 0, 0, 5, 4 };

MazeLevel level1;

bool powerUpTime = false;
float pacManStep = 10.0;

//bool inGame = true;
std::atomic<bool> inGame(false);

mutex mutexGame;
mutex mutexTest;

int masterMazeData[22][19];				//int iMazeData[22][19];

int playerLives = 3;

bool levelEnd = false;

auto powerUpTimerStart = chrono::high_resolution_clock::now();

chrono::high_resolution_clock::time_point checkTime;
chrono::high_resolution_clock::time_point::duration duration;
double elapsedTime;

chrono::high_resolution_clock::time_point gameStartTime;
//auto lastLeft = std::chrono::high_resolution_clock::now();

map<int, bool> keys;

void renderScene(void);
void keyboard(unsigned char key, int x, int y);
bool wallInterference(int x, int y);
void changeSize(int w, int h);
void processNormalKeys(unsigned char key, int xx, int yy);
void pressSpecialKey(int key, int xx, int yy);
void releaseSpecialKey(int key, int x, int y);
void setInitialValues();
bool playerGhostInterference();
void resetStartPositions();

//if( something == MazeLevel::mazeType::coin)

//************************************************************************
// Up, Down, Left, Right movement of pac-man around the grid
void pressSpecialKey(int key, int xx, int yy)
{
//	mutexGame.lock();
	keys[key] = true;	// key has been pressed
	inGame = true;		// game can start
//	mutexGame.lock();

}
//************************************************************************
// Up, Down, Left, Right movement of pac-man around the grid
void releaseSpecialKey(int key, int x, int y)
{
	keys[key] = false;		// key is not pressed anymore
}
//************************************************************************
// Stuff to initialize before the game begins and renders
void initialize()
{
	// initialize level matrix data
	level1.clearLevel();					// clear the level values
	level1.readFile("MazeLevel1.txt");		// read input text file of values used to build level

	// give maze data to ghosts so they can know where walls are
	for (int ii = 0; ii < 22; ++ii)				// from 0 to number of rows, y
	{
		for (int jj = 0; jj < 19; ++jj)			// from 0 to the number of columns, x
		{
			rGhost.pathMatrix[ii][jj] = level1.iLevelArray[ii][jj];
			gGhost.pathMatrix[ii][jj] = level1.iLevelArray[ii][jj];
			pGhost.pathMatrix[ii][jj] = level1.iLevelArray[ii][jj];
			oGhost.pathMatrix[ii][jj] = level1.iLevelArray[ii][jj];
//			cout << rGhost.pathMatrix[ii][jj];
		}
//		cout << endl;
	}
//	cout << endl;
	rGhost.setMazeData();
	gGhost.setMazeData();
	pGhost.setMazeData();
	oGhost.setMazeData();

	setInitialValues();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_NORMALIZE);

	// from original init() function
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);

	// improve speed by rendering only what's visible
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// set up camera
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLoadIdentity();
	eyeX = centerX + RADIUS * cos(i*rotation);
	eyeY = centerY + RADIUS * sin(i*rotation);
	gluLookAt(
		eyeX, eyeY, cameraZ,
		centerX, centerY, centerZ,
		0.0, 0.0, 1.0);

}

//*************************************************************
// Called at the start of renderScene(), this handles all the 
// updating of critical game play variables
void update()
{
	// move the checks for pac-man's position from keys to here
	// if player pressed an arrow key and if there is no interference with a maze wall in this direction, then update location
	if (keys[GLUT_KEY_UP])
	{
		if (wallInterference(0, 1) == false)
		{
			player.location[1] += pacManStep;
		//	this_thread::sleep_for(100ms);
		}
	}
	else if (keys[GLUT_KEY_LEFT])
	{
		if (wallInterference(-1, 0) == false)
		{
			player.location[0] -= pacManStep;
		//	this_thread::sleep_for(100ms);
		}
		if (player.location[0] == 0 && player.location[1] == 110)
		{
			player.location[0] = 190;
		}
	}
	else if (keys[GLUT_KEY_DOWN])
	{
		if (wallInterference(0, -1) == false)
		{
			player.location[1] -= pacManStep;
		//	this_thread::sleep_for(100ms);
		}
	}
	else if (keys[GLUT_KEY_RIGHT])
	{
		if (wallInterference(1, 0) == false)
		{
			player.location[0] += pacManStep;
		//	this_thread::sleep_for(100ms);
		}
		if (player.location[0] == 180 && player.location[1] == 110)
		{
			player.location[0] = 0;
		}
	}

	// check if pac-man's in power-up mode to change ghost colors
	if (powerUpTime == false)
	{
		rGhost.isVulnerable = false;
		gGhost.isVulnerable = false;
		pGhost.isVulnerable = false;
		oGhost.isVulnerable = false;
	}
	else
	{
		rGhost.isVulnerable = true;
		gGhost.isVulnerable = true;
		pGhost.isVulnerable = true;
		oGhost.isVulnerable = true;
	}

	int xPacMan = (int)player.location[0] / 10;			// Pac-Man's x position
	int yPacMan = (int)player.location[1] / 10;			// Pac-Man's y position

	// check if pac-man ate a pac-dot (update pac-dots matrix)
	if (level1.pacDotsArray[yPacMan][xPacMan] == 1)	
	{
		level1.pacDotsArray[yPacMan][xPacMan] = 0;
	}
	// check if pac-man got a power-up (update)
	if (level1.powerUpsArray[yPacMan][xPacMan] == 1)
	{
		level1.powerUpsArray[yPacMan][xPacMan] = 0;
		powerUpTime = true;	
		powerUpTimerStart = chrono::high_resolution_clock::now();
//		player.poweredUp = true;
	}
//	teleportCheckGhost(&rGhost);
//	teleportCheckPlayer(&player);

	// power-up timer check
	checkTime = chrono::high_resolution_clock::now();
	duration = chrono::duration_cast<chrono::nanoseconds>(checkTime - powerUpTimerStart);
	elapsedTime = double(duration.count()) / 1000000000;

	playerGhostInterference();
	if (elapsedTime > 6.0)
	{
		powerUpTime = false;
	}
	if (playerLives < 0)
	{
		inGame = false;
		initialize();
//		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//		inGame = true;
		playerLives = 3;
	}

}

//************************************************************************
// Draw the entire scene after updating game variables
void renderScene(void)
{
	// make sure all variables are updated before rendering
	update();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Clear color and depth buffers
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera set up
	eyeX = centerX + RADIUS * cos(i*rotation);
	eyeY = centerY + RADIUS * sin(i*rotation);
	gluLookAt(
		eyeX, eyeY, cameraZ,
		centerX, centerY, centerZ,
		0.0, 0.0, 1.0);
	//	init();

	// Draw Stuff
	glPushMatrix();							// push to save location
											//glRotatef((GLfloat)mazeRotate, 0, 0, 1); // rotate map when user pushes R or r
											//glTranslatef(centerX, centerY, 0.0);
											// Draw the scene, pac-man, and ghosts
											// i would like to figure out how to use display lists for these objects
	level1.render();
	player.render();

	// render ghost if it's alive
	if (rGhost.isAlive)
	{
		rGhost.render(quadratic);
	}
	if (gGhost.isAlive)
	{
		gGhost.render(quadratic);
	}
	if (pGhost.isAlive)
	{
		pGhost.render(quadratic);
	}
	if (oGhost.isAlive)
	{
		oGhost.render(quadratic);
	}

	glPopMatrix();

	// Make it all visible
	glutSwapBuffers(); 
	std::this_thread::sleep_for(std::chrono::milliseconds(80));
	glutPostRedisplay();
}

//************************************************************************
// Main program  - standard GLUT initializations and callbacks
int main(int argc, char **argv)
{
	printf("\n\
-----------------------------------------------------------------------\n\
  ECE 6122 Lab 4 Pac-Man Project:\n\
  - Press R or r to rotate the maze clock-wise\n\
  - Press the arrow keys to move Pac-Man around the maze\n\
  - q or ESC to quit\n\
-----------------------------------------------------------------------\n");

	// general initializations
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 400);
	glutCreateWindow("Millenial Pac-Man");

	// stuff to initialize before starting the rendering and game play
	initialize();

	//glEnable(GL_DEPTH_TEST);

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIgnoreKeyRepeat(1);					// ignore key repeat when holding key down
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(pressSpecialKey);	
	glutSpecialUpFunc(releaseSpecialKey);
	glutMainLoop();
	return 0; 
}


//************************************************************************
// Check player/ghost interference to determine playerLives update, 
// and ghost bool updates
bool playerGhostInterference()
{
	if (player.location[0] == rGhost.location[0] && player.location[1] == rGhost.location[1])
	{
		if (rGhost.isVulnerable == true)
		{
	//		mutexTest.lock();
			rGhost.isAlive = false;
	//		mutexTest.unlock();
//			cout << "Red ghost should die!" << endl;
//			rGhost.isVulnerable = false;
		}
		else
		{
	//		mutexTest.lock();
//			rGhost.isAlive = false;
			playerLives--;
//			cout << "Pac Man should die!" << endl;
			resetStartPositions();
	//		mutexTest.unlock();
		}
		return true;
	}
	if (player.location[0] == gGhost.location[0] && player.location[1] == gGhost.location[1])
	{
		if (gGhost.isVulnerable == true)
		{
			gGhost.isAlive = false;
//			cout << "Green ghost should die!" << endl;
//			gGhost.isVulnerable = false;
		}
		else
		{
//			gGhost.isAlive = false;
			playerLives--;
//			cout << "Pac Man should die!" << endl;
			resetStartPositions();
		}
		return true;
	}
	if (player.location[0] == pGhost.location[0] && player.location[1] == pGhost.location[1])
	{
		if (pGhost.isVulnerable == true)
		{
			pGhost.isAlive = false;
//			cout << "Pink ghost should die!" << endl;
		}
		else
		{
//			pGhost.isAlive = false;
			playerLives--;
//			cout << "Pac Man should die!" << endl;
			resetStartPositions();
		}
		return true;
	}
	if (player.location[0] == oGhost.location[0] && player.location[1] == oGhost.location[1])
	{
		if (oGhost.isVulnerable == true)
		{
			oGhost.isAlive = false;
//			cout << "Orange ghost should die!" << endl;
		}
		else
		{
//			oGhost.isAlive = false;
			playerLives--;
//			cout << "Pac Man should die!" << endl;
			resetStartPositions();
		}
		return true;
	}
	return false;
}

//************************************************************************
// Reset pac-man's location and set ghost bools that trigger them to respawn
void resetStartPositions()
{
//	setInitialValues();
//	player.setColor(1.0, 1.0, 0.0, 1.0);
	player.setLocation(90.0, 50.0, 0.0);
	rGhost.playerDeath = true;
	gGhost.playerDeath = true;
	pGhost.playerDeath = true;
	oGhost.playerDeath = true;
//	rGhost.setLocation(90.0, 120.0, 0.0);   //(90.0, 115.0, 0.0);
//	gGhost.setLocation(80.0, 110.0, 0.0);   //(80.0, 105.0, 0.0);
//	pGhost.setLocation(90.0, 110.0, 0.0);   //(90.0, 105.0, 0.0);
//	oGhost.setLocation(100.0, 110.0, 0.0);  //(100.0, 105.0, 0.0);
//	rGhost.setColor();
//	gGhost.setColor();
//	pGhost.setColor();
//	oGhost.setColor();
//	rGhost.isVulnerable = false;
//	gGhost.isVulnerable = false;
//	pGhost.isVulnerable = false;
//	oGhost.isVulnerable = false;

//	rGhost.isAlive = true;
//	gGhost.isAlive = true;
//	pGhost.isAlive = true;
//	oGhost.isAlive = true;
}


//----------------------------------------------------------------------
// Rotate maze when key pressed is 'R' or 'r':
//		the global variable mazeRotate gets updated by 5 
//		(for if the rotation is done by moving the entire scene), 
//		and i updated by 1 (for parametric equations for camera movement)
//----------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r':
		mazeRotate += 5;
		i += 1;
		glutPostRedisplay();
		break;
	case 'R':
		mazeRotate += 5;
		i += 1;
		glutPostRedisplay();
		break;
	default:
//		inGame = true;
		break;
	}
}
//----------------------------------------------------------------------
// Check for interference of pac-man with the walls by taking in the
// the potential movement directions of pac-man to see where he can
//	INPUT: X-direction and Y-direction of pac-man's potential movement
//  OUTPUT: true (interference) or false (pac-man can move this direction)
//----------------------------------------------------------------------
bool wallInterference(int x, int y)
{
	int xCheck = (int)player.location[0] / 10 + x;
	int yCheck = (int)player.location[1] / 10 + y;

	//cout << "X Check: " << xCheck << endl << "Y Check: " << yCheck << endl;

	// if his desired movement directon interferes with a wall
	if (level1.wallsArray[yCheck][xCheck] == 1)
	{
		// there is an interference
		return true;
	}
	else
	{
		// there is no interference and he can move in this direction
		return false;
	}
}

//----------------------------------------------------------------------
// Reshape callback -  changing size of the window
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//----------------------------------------------------------------------
void changeSize(int w, int h)
{
	float ratio = ((float)w) / ((float)h); // window aspect ratio
	glMatrixMode(GL_PROJECTION); // projection matrix is active
	glLoadIdentity(); // reset the projection
	gluPerspective(15.0, ratio, 0.1, 2000.0); //(60.0, ratio, 0.1, 1000000.0); // perspective transformation
	glMatrixMode(GL_MODELVIEW); // return to modelview mode
	glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// User-input callbacks
// processNormalKeys: ESC, q, and Q cause program to exit
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q')
	{
		exit(0);
	}
}

//----------------------------------------------------------------------
// Initialize the locations and colors of pac-man and ghosts
//----------------------------------------------------------------------
void setInitialValues()
{
	player.setColor(1.0, 1.0, 0.0, 1.0);
	player.setLocation(90.0, 50.0, 0.0);
	rGhost.setLocation(90.0, 120.0, 0.0);   //(90.0, 115.0, 0.0);
	gGhost.setLocation(80.0, 110.0, 0.0);   //(80.0, 105.0, 0.0);
	pGhost.setLocation(90.0, 110.0, 0.0);   //(90.0, 105.0, 0.0);
	oGhost.setLocation(100.0, 110.0, 0.0);  //(100.0, 105.0, 0.0);
	rGhost.setColor();
	gGhost.setColor();
	pGhost.setColor();
	oGhost.setColor();
	rGhost.isVulnerable = false;
	gGhost.isVulnerable = false;
	pGhost.isVulnerable = false;
	oGhost.isVulnerable = false;
	rGhost.isAlive = true;
	gGhost.isAlive = true;
	pGhost.isAlive = true;
	oGhost.isAlive = true;
}