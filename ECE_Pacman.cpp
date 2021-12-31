/*
Author: Deanna Gierzak
Class: ECE 6122
Last Date Modified: 12/01/2020

Description:

This ECE_Pacman class handles the setting and rendering of
Pac-Man and his properties

*/


#include "ECE_Pacman.h"
#include "ECE_Bitmap.h"

GLUquadricObj *quadratic5 = gluNewQuadric();



////////////////////////////////////////////////////////////////////
// FUNCTION: RENDER PAC-MAN
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// Renders pac-man as a yellow sphere
void ECE_Pacman::render()
{
	float colorAmb[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	float colorDiff[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	float colorSpec[4] = { 0.05f, 0.05f, 0.05f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colorDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorSpec);

	/*
	glColor3f(color[0], color[1], color[2]);
	glPushMatrix();
	glTranslatef(location[0], location[1], location[2]);
	glutSolidSphere(5.0, 30, 30); // draw pac-man as a sphere
	glPopMatrix();
	*/
	//myinit2();

	// texture wrapping stuff commented out because it didn't work 
	glPushMatrix();
	glTranslatef(location[0], location[1], location[2]);	// translate to location defined by member variable
															//glBindTexture(GL_TEXTURE_2D, texture[0]);
	glRotatef(-180.0f, -0.01f, 0.9f, 0.01f);				// rotate, for purpose of texture
															//gluQuadricTexture(quadratic5, GL_TRUE);
	gluSphere(quadratic5, 5.0, 30, 30);						// create a gluSphere for texture wrap
															//glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix(); //pop exits location

}

////////////////////////////////////////////////////////////////////
// FUNCTION: SET LOCATION OF PAC-MAN
//	INPUT: X, Y, Z location of pac-man in grid
//  OUTPUT: NONE
//****************************************************************
// Set pac-man's location in the maze with inputs
void ECE_Pacman::setLocation(float locationX, float locationY, float locationZ)
{
	this->location[0] = locationX;
	this->location[1] = locationY;
	this->location[2] = locationZ;
}


////////////////////////////////////////////////////////////////////
// not currently used, but tentative function for if desired to
//		change pac-man's color
////////////////////////////////////////////////////////////////////
// FUNCTION: SET COLOR OF PAC-MAN
//	INPUT: R, G, B, Alpha values 
//  OUTPUT: NONE
//****************************************************************
// Sets the values of color[] array for pac-man
void ECE_Pacman::setColor(float colorR, float colorG, float colorB, float colorAlpha)
{
	this->color[0] = colorR;
	this->color[1] = colorG;
	this->color[2] = colorB;
	this->color[3] = colorAlpha;
}
////////////////////////////////////////////////////////////////////
// not currently used, but tentative function for final project
////////////////////////////////////////////////////////////////////
// FUNCTION: GIVE PAC-MAN POWER-UP ABILITIES TO EAT GHOSTS
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// Allow pac-man to 
void ECE_Pacman::superPower()
{
	// give pac-man the ability to kill ghosts for 5 seconds
	// change ghost color to white during this time
}