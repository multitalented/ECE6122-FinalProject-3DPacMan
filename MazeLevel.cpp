/*
Author: Deanna Gierzak
Class: ECE 6122
Last Date Modified: 10/28/2020

Description:

	See MazeLevel.h file for detailed overall explanation

*/

#include "MazeLevel.h"

GLUquadricObj *quadratic4 = gluNewQuadric();

////////////////////////////////////////////////////////////////////
// FUNCTION: READ A FILE OF VALUES THAT WILL BE USED FOR SCENE CREATION
//	INPUT: .txt FILE THAT CONTAINS VALUES OF W, D, P (wall, pac-dot, power-up)
//  OUTPUT: NONE
//****************************************************************
// The .txt file is used to fill the boolean 0/1 values for wallsArray, 
//		pacDotsArray, and powerUpsArray with W, D, and P respectively.
void MazeLevel::readFile(const string& filename)
{
	char val = 0, k = 0;
	ifstream thisFile(filename);
	//vector<char> gridData;
	char dummy[2] = { 0,0 };
	string dataline;
	
	// go through every line in the file to collect the characters W, D, P
	for (int ii = 0; ii < 22; ++ii)
	{
		getline(thisFile, dataline);

		cout << dataline << endl;
		
		for (int jj = 0; jj < 19; ++jj)
		{
			dummy[0] = dataline[jj];
			this->levelArray[ii][jj] = dummy[0];
		}
	}



	// go through the char array of level data to create the boolean arrays for walls (W), pac-dots (D), and power-ups (P)
	for (long i = 0; i < 22; ++i)		// from 0 to number of rows
	{
		for (long j = 0; j < 19; ++j)	// from 0 to the number of columns
		{
			switch (levelArray[i][j])
			{
				case 'W':
					iLevelArray[i][j] = 0;	// 0 is wall
					//mazeArray[i][j] = wall;
					wallsArray[i][j] = 1;		// place a 1 (true) to indicate a wall is located at this point in the grid
					pacDotsArray[i][j] = 0;
					powerUpsArray[i][j] = 0;
					break;
				case 'D':
					iLevelArray[i][j] = 1;	// 1 is coin/pac-dots
					//mazeArray[i][j] = coin;
					pacDotsArray[i][j] = 1;		// place a 1 (true) to indicate a pac-dot is located at this point in the grid
					wallsArray[i][j] = 0;
					powerUpsArray[i][j] = 0;
					break;
				case 'P':
					iLevelArray[i][j] = 2;	// 2 is power-up
					//mazeArray[i][j] = power;
					powerUpsArray[i][j] = 1;	// place a 1 (true) to indicate a power-up is located at this point in the grid
					wallsArray[i][j] = 0;
					pacDotsArray[i][j] = 0;
					break;
				default:
					iLevelArray[i][j] = 3;	// 3 will indicate it is an open space to move
					powerUpsArray[i][j] = 0;
					wallsArray[i][j] = 0;
					pacDotsArray[i][j] = 0;
					break;
			}
		}
	}

	// from the wallsArray, go through to find which should be used to create xWallsArray
	for (long i = 0; i < 22; ++i)		// ( y direction) from 0 to number of rows
	{
		for (long j = 0; j < 19; ++j)	// (x direction) from 0 to the number of columns
		{
			// if there is a wall at this location in the grid
			if (this->wallsArray[i][j] == 1)
			{
				// first two if statement checks have to happen first to avoid out-of-bounds checking of the array
				// if the "left" side of this value is outside the range
				if (j - 1 < 0)
				{
					// and the "right" side is 0
					if (this->wallsArray[i][j + 1] == 0)
					{
						// then this should not be a point to create a wall in the x-direction
						this->xWallsArray[i][j] = 0;
					}
					else
					{
						// otherwise if the value on the "right" side is 1, then this indicates a valid point for drawing a wall in the x-direction
						this->xWallsArray[i][j] = 1;
					}
				}
				// if the "right" side of this value is outside the range
				else if (j + 1 > 18)
				{
					// and the "left" side is 0
					if (this->wallsArray[i][j - 1] == 0)
					{
						// then this should not be a point to create a wall in the x-direction
						this->xWallsArray[i][j] = 0;
					}
					else
					{
						// otherwise if the value on the "left" side is 1, then this indicates a valid point for drawing a wall in the x-direction
						this->xWallsArray[i][j] = 1;
					}
				}
				// if the "left" side of this value and the "right" side of this value are both equal to 0
				else if (this->wallsArray[i][j - 1] == 0 && this->wallsArray[i][j + 1] == 0)
				{
					// then this is not a valid point to place a x-direction wall 
					this->xWallsArray[i][j] = 0;
				}
				else
				{
					// otherwise, safe to place a x-direction wall here!
					this->xWallsArray[i][j] = 1;
				}
			}
		}
	}

	// from the wallsArray, go through to find which should be used to create yWallsArray
	for (long j = 0; j < 19; ++j)		// ( y direction) from 0 to number of rows
	{
		for (long i = 0; i < 22; ++i)	// (x direction) from 0 to the number of columns
		{
			// if there is a wall at this location in the grid
			if (this->wallsArray[i][j] == 1)
			{
				// first two if statement checks have to happen first to avoid out-of-bounds checking of the array
				// if the "left" side of this value is outside the range
				if (i - 1 < 0)
				{
					// and the "right" side is 0
					if (this->wallsArray[i + 1][j] == 0)
					{
						// then this should not be a point to create a wall in the y-direction
						this->yWallsArray[i][j] = 0;
					}
					else
					{
						// otherwise if the value on the "right" side is 1, then this indicates a valid point for drawing a wall in the y-direction
						this->yWallsArray[i][j] = 1;
					}
				}
				// if the "right" side of this value is outside the range
				else if (i + 1 > 22)
				{
					// and the "left" side is 0
					if (this->wallsArray[i - 1][j] == 0)
					{
						// then this should not be a point to create a wall in the y-direction
						this->yWallsArray[i][j] = 0;
					}
					else
					{
						// otherwise if the value on the "left" side is 1, then this indicates a valid point for drawing a wall in the y-direction
						this->yWallsArray[i][j] = 1;
					}
				}
				// if the "left" side of this value and the "right" side of this value are both equal to 0
				else if (this->wallsArray[i - 1][j] == 0 && this->wallsArray[i + 1][j] == 0)
				{
					// then this is not a valid point to place a y-direction wall 
					this->yWallsArray[i][j] = 0;
				}
				else
				{
					// otherwise, safe to place a y-direction wall here!
					this->yWallsArray[i][j] = 1;
				}
			}
		}
	}
	// finally, determine where the corners are from the final xWallsArray and yWallsArray (where there is overlap of positions)
	// from the wallsArray, go through to find which should be used to create xWallsArray
	for (long i = 0; i < 22; ++i)		// ( y direction) from 0 to number of rows
	{
		for (long j = 0; j < 19; ++j)	// (x direction) from 0 to the number of columns
		{
			// if there is both an x-direction wall and a y-direction wall here, then a rounded corner may be required
			if (this->xWallsArray[i][j] == 1 && this->yWallsArray[i][j] == 1)
			{
				wallCornersArray[i][j] = 1;
			}
			else
			{
				wallCornersArray[i][j] = 0;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// FUNCTION: CLEAR ALL THE SCENE OBJECT ARRAYS TO ZERO
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// An initialization routine to ensure cleared arrays before 
//		building a new level. Resets all int arrays to 0
//		char levelArray to O (for nothing), and score to 0
void MazeLevel::clearLevel()
{
	for (int ii = 0; ii < 22; ++ii)
	{
		for (int jj = 0; jj < 19; ++jj)
		{
			this->levelArray[ii][jj] = 'O';
			this->wallsArray[ii][jj] = 0;
			this->pacDotsArray[ii][jj] = 0;
			this->powerUpsArray[ii][jj] = 0;
			this->xWallsArray[ii][jj] = 0;	
			this->yWallsArray[ii][jj] = 0;	
			this->wallCornersArray[ii][jj] = 0;	
		}
		cout << endl;
	}
	score = 0;
}

////////////////////////////////////////////////////////////////////
// FUNCTION: RENDER THE SCENE FOR THE LEVEL
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// An initialization routine that renders the walls, pac-dots,
//		and power-ups in the scene
void MazeLevel::render()
{
	renderMazeWalls();
	renderMazeWallCorners();
	renderPacDots();
	renderPowerUps();
}

////////////////////////////////////////////////////////////////////
// FUNCTION: RENDER THE PAC-DOTS
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// The routine that renders the pac-dots
void MazeLevel::renderPacDots()
{
	// Set the pac-dot material/colors
//	float colorWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	float ambWhite[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
//	float diffWhite[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
//	float specWhite[4] = { 0.03f, 0.03f, 0.03f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambWhite);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffWhite);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specWhite);

	// Draw the pac-dots
	for (long i = 0; i < 22; ++i)		// from 0 to number of rows (y)
	{
		for (long j = 0; j < 19; ++j)	// from 0 to the number of columns (x)
		{
			if (pacDotsArray[i][j] == 1)
			{
				glPushMatrix();
					glTranslatef(10.0*j, 10.0*i, 0.0);
					glutSolidSphere(pacDotRadius, 20, 20);	// draw the pac-dot as a small white sphere
				glPopMatrix();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// FUNCTION: RENDER THE POWER-UPs
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// The routine that renders the power-ups
void MazeLevel::renderPowerUps()
{
	// Set the power-up material/colors
	float colorGold[4] = { 0.855f, 0.647f, 0.125f, 1.0f };
	float ambGold[4] = { 0.855f, 0.647f, 0.125f, 1.0f };
	float diffGold[4] = { 0.855f, 0.647f, 0.125f, 1.0f };
	float specGold[4] = { 0.05f, 0.05f, 0.05f, 1.0f }; //{ 1.0, 1.0, 0.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambGold);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffGold);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specGold);

	// Draw the power-ups
	for (long i = 0; i < 22; ++i)		// from 0 to number of rows (y)
	{
		for (long j = 0; j < 19; ++j)	// from 0 to the number of columns (x)
		{
			if (powerUpsArray[i][j] == 1)
			{
				glPushMatrix();
					glTranslatef(10.0f*j, 10.0f*i, 0.0f);
					//gluCylinder(quadratic4, powerUpRadius, powerUpRadius, 2.0, 32, 32); // make the cylindrical disc part
					//glTranslatef(0.0f, 0.0f, 2.0f);
					glScalef(4.0f, 4.0f, 1.0f); // scale sphere into a flat pancake (top of the disc)
					glutSolidSphere(powerUpRadius / 4, 20, 20);		// draw power-ups as gold flattened spheres
				glPopMatrix();

			}
		}
	}

}

////////////////////////////////////////////////////////////////////
// FUNCTION: RENDER THE MAZE WALLS
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// The routine that renders the walls of the maze
void MazeLevel::renderMazeWalls()
{
	// Set the walls' material/colors
	float colorBlue[4] = { 0.0f, 0.749f, 1.0f, 1.0f };
	float ambBlue[4] = { 0.118f, 0.565f, 1.0f, 1.0f }; //{ 0.0f, 0.749f, 1.0f, 1.0f };
	float diffBlue[4] = { 0.1f, 0.549f, 1.0f, 1.0f }; //{ 0.f, 0.749f, 1.0f, 1.0f };
	float specBlue[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambBlue);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffBlue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specBlue);
	float xStart, xLoc; //xEnd;
	float yStart, yLoc;
	double height;
	long k = 0;

	// Draw the walls parallel to the x-axis
	// Important here to go through by row, column order, so that the internal 
	//		for-loop search is occurring in the x-direction (columns)
	for (long i = 0; i < 22; ++i)		// (y direction) from 0 to number of rows
	{
		for (long j = 0; j < 19; ++j)	// (x direction) from 0 to the number of columns
		{
			if (this->xWallsArray[i][j] == 1)		// there is a wall in the x-direction at this point in the grid
			{
				xLoc = j * 10.0; yLoc = i * 10.0;
				xStart = j * 10.0;
				for (j; j < 19; ++j)				// check remaining points after this ones
				{
					if (this->xWallsArray[i][j] == 1)	// there is a wall here too! Draw wall
					{
						height = j * 10.0 - xStart;		// determine the height (length) of the cylinder for wall

						glPushMatrix();	// push to save location
							glTranslatef(xLoc, yLoc, 0.0f);	// translate along the y-axis
							glRotatef(90.0f, 0.0f, 1.0f, 0.0f);	// rotate 90 degrees about the y-axis
							gluCylinder(quadratic4, wallCylinderRadius, wallCylinderRadius, height, 32, 32);	// create a cylinder with radius defined by wallCylinderR and length defined as above
						glPopMatrix();	// pop to return to location
					}
					else  // there is no wall, don't draw anything!
					{
						break;
					}
				}
			}
		}
	}

	// Draw the walls parallel to the y-axis
	// Important here to go through by column, then row order, so that the internal 
	//		for-loop search is occurring in the y-direction (rows)
	for (long j = 0; j < 19; ++j)	// (x direction) from 0 to the number of columns
	{
		for (long i = 0; i < 22; ++i)		// (y direction) from 0 to number of rows
		{
			if (this->yWallsArray[i][j] == 1)		// there is a wall in the y-direction at this point in the grid
			{
				xLoc = j * 10.0; yLoc = i * 10.0;
				yStart = i * 10.0;

				for (i; i < 22; ++i)		// check remaining points after this ones
				{
					if (this->yWallsArray[i][j] == 1) // there is a wall here too! Draw wall
					{
						height = i * 10.0 - yStart;

						glPushMatrix(); // push to save location
							glTranslatef(xLoc, yLoc, 0.0f);	// translate along the y-axis
							glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);	// rotate 90 degrees about the y-axis
							gluCylinder(quadratic4, wallCylinderRadius, wallCylinderRadius, height, 32, 32);	// create a cylinder with radius defined by wallCylinderR and length defined above
						glPopMatrix();	// pop to return to location
					}
					else   // there is no wall, don't draw anything!
					{
						break;
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// FUNCTION: RENDER THE ROUNDED MAZE WALL CORNERs
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// The routine that renders corners of the maze walls for smooth appearance
void MazeLevel::renderMazeWallCorners()
{
	float colorBlue[4] = { 0.0f, 0.749f, 1.0f, 1.0f };
	float ambBlue[4] = { 0.118f, 0.565f, 1.0f, 1.0f }; //{ 0.0f, 0.749f, 1.0f, 1.0f };
	float diffBlue[4] = { 0.1f, 0.549f, 1.0f, 1.0f }; //{ 0.f, 0.749f, 1.0f, 1.0f };
	float specBlue[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambBlue);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffBlue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specBlue);

	// Draw the rounded corners of the walls
	for (long i = 0; i < 22; ++i)		// from 0 to number of rows, y
	{
		for (long j = 0; j < 19; ++j)	// from 0 to the number of columns, x
		{
			if (wallCornersArray[i][j] == 1)
			{
				glPushMatrix();
					glTranslatef(10.0f*j, 10.0f*i, 0.0f);
					glutSolidSphere(wallCylinderRadius, 20, 20);
				glPopMatrix();

			}
		}
	}

}
////////////////////////////////////////////////////////////////////
// not currently used, tentative function, not sure yet if necessary
////////////////////////////////////////////////////////////////////
// FUNCTION: RENDER ONLY OBJECTS THAT WILL CHANGE DURING PLAY-TIME
//	INPUT: NONE
//  OUTPUT: NONE
//****************************************************************
// Render changed pac-dots and power-ups as Pac-Man moves
void MazeLevel::renderChanges()
{
	renderPacDots();
	renderPowerUps();
}
