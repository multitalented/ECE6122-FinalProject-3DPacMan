/*
Author: Deanna Gierzak
Class: ECE 6122
Last Date Modified: 10/28/2020

Description:

This ECE_Pacman class handles the setting and rendering of
Pac-Man and his properties

*/

#pragma once
//#include <GL/glew.h> // Include the GLEW header file
//#include <conio.h>
#include <iostream>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // standard definitions
//using namespace std;



class ECE_Pacman
{
private:
	//float color[4];
	//float location[3];
public:
	float color[4];				// holds the RGBA color of pac-man (not currently used, right now using a default yellow set within render)
	float location[3];			// holds the X, Y, Z location of pac-man

	bool alive = true;
	bool poweredUp = false;


//	float size;					// size of pac-man, not currently used
	void render();				// renders pac-man
	void setColor(float colorR, float colorG, float colorB, float colorAlpha);	// sets a color for pac-man (not currently used in main)
	void setLocation(float locationX, float locationY, float locationZ);		// sets the location fo pac-man in X-Y-Z coordinate system
	void superPower();			// activate pac-man's power-up abilities

	//static int iMazeData[22][19];


	float colorYellow[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	float ambYellow[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	float diffYellow[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	float specYellow[4] = { 0.05f, 0.05f, 0.00f, 1.0f };

};