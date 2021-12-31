/*
Author: Deanna Gierzak
Class: ECE 6122
Last Date Modified: 12/01/2020

Description:

	This ECE_Ghost class handles the setting and rendering of a ghost, its properties,
	and its behavior.

	The ghost colors are set by an enum name of red, orange, pink, and green. These are
	determined when defined in main.cpp like follows:

	ECE_Ghost colorGhost(colorName);

	Based on the colorName of the ghost


	Each ghost runs on its thread, and the thread starts once the constructor is called
	when a ghost is defined. The constructor sends the ghost thread into threadFunc, a
	friend function where the thread runs in a loop, with logic to determine what the
	ghost should do based on whether the game is started or waiting to start.

	The thread determines when to respawn the ghost, let the ghost exit the home base,
	and when to "play" [ghostGamePlay()].

	When the thread goes into ghostGamePlay, the ghosts either chase Pac-Man [chaseByVector()]
	or scatter [scatter()] based on whether they are vulnerable [bool isVulnerable].

	In chaseByVector(), the ghosts use Pac-Man's location and their location to check
	the shortest x/y distances (dx, dy). In cases of walls blocking their direct path, they choose
	a valid direction randomly. In cases of equal dx/dy, they pick a valid direction somewhat randomly.

	In scatter(), the ghosts move in a random direction, and then maintain that direction until
	they hit a wall, and then choose another random direction, and so on.

*/

#pragma once
//#include <GL/glew.h> // Include the GLEW header file
//#include <conio.h>
#include <iostream>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // standard definitions
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <atomic>
#include <map>
#include <queue>

enum ghostColor { red, green, pink, orange };

// these values can be used to fill an array of information that each ghost can use to determine where they can move and how to find target (player/pacman)
//enum gridDataType { wall, empty, target, path };	

//using namespace std;

class ECE_Ghost
{
public:
//	static gridDataType mazeData[22][19];
//	static int intMazeData[22][19];
	int pathMatrix[22][19];		 // [y grid max][x grid max] or [rows][cols]

	float targetLoc[2];				// not used
	float elapsedTime = 0.0016;		// not used
	float velocity = 30;			// not used

	float xStart;					// starting X location of ghost
	float yStart;					// starting Y location of ghost

	float xNextBFS;					// not used
	float yNextBFS;					// not used

//	vector<queueNode> pathVector;
	//vector<int> vect;

	ECE_Ghost(enum ghostColor color);	// main constructor
	ECE_Ghost();
	//~ECE_Ghost();

	void ghostGamePlay();			// determines whether to chase or scatter
	
	void chaseByVector();			// determines which direction to go based on ghost location and pac-man's location
	void chaseByBFS();				// not used
	void scatter();					// determines which random direction to take

//	int BFS(int mat[][19], Point src, Point dest);


	void render(GLUquadricObj* quadr);				// renders the ghost as sphere and cylinder with its defined colors and eye-texture wrap
	void getTargetLocation(float x, float y);		// not used
	
	//void setGhostColorType(ghostColor color);
	void setColor();
	void setSpecColor(float inColor[4]);	// sets the color[] private member variable of a ghost
	void setAmbColor(float incolor[4]);		// sets the ambColor[] private member variable of a ghost
	void setDiffColor(float incolor[4]);	// sets the diffColor[] private member variable of a ghost
	void setLocation(float locationX, float locationY, float locationZ);		// sets the location[] private member variable of a ghost
	
	// not used yet
	void wanderTimer();			// not used

	void ghostNormal();			// not used, set ghosts to their normal color values as defined by private member variables
	void ghostWhite();			// not used, set ghosts to white
	void respawn();				// respawn ghosts at starting location and reset member variables
	void setMazeData();			// set maze data from the MazeLevel matrix to 1/0's for ghosts to use to determine valid places to move
	void resetLocation();		// not used
	void exitHome();			// ghost exits home base and pathMatrix is updated to reflect "invisible" walls so that the ghost cannot go back in


	bool isVulnerable = false;	// tells when the ghost is vulnerable from pac-man powerup
	bool isAlive = true;		// tells when the ghost has been eaten
	bool isReleased = false;	// tells whether the ghost is released into the maze
	bool playerDeath = false;	// tells ghost whether the player has lost all their lives

	float location[3];			// holds the X, Y, Z location of the ghost
	
	std::mutex mutexRelease;
//	std::mutex mutexTest;
//	std::mutex mutexGame2;

	std::chrono::milliseconds waitTime;			// used to give ghosts different release times
	std::chrono::milliseconds ghostSpeed;		// used to set the speed of each ghost so they don't overlap as badly
	
protected:											

private:
	
	std::thread startGhostThread;			// the thread for the ghost to run in
	ghostColor thisGhostColor;				// the color of the ghost set in the constructor
	
	float color2D[4];			// holds the RGBA color of ghost
	float ambColor[4];			// holds the RGBA color to use for ambient lighting of ghost
	float diffColor[4];			// holds the RGBA color to use for diffuse lighting of ghost
	float specColor[4];			// holds the RGBA color to use for specular lighting of ghost, all defined to have the same
	
//	float size;					// holds the size of ghost
	float colorWhiteGhost[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float ambWhiteGhost[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float diffWhiteGhost[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float specWhiteGhost[4] = { 0.03f, 0.03f, 0.03f, 1.0f };
	
//	static int iMazeData[22][19];
	//bool wanderMode = false;			// tell whether ghost is in wanderMode or not
	//static float targetLoc[2];
	//	static int iMazeData[2][2];
	// int iMazeData[2][2] = { (1, 1), (1, 1) };
	//void ghostGamePlay(ECE_Ghost*thisGhost);
};

//int ECE_Ghost::iMazeData[22][19] = {};
