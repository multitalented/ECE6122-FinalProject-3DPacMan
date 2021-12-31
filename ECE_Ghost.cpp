/*
Author: Deanna Gierzak
Class: ECE 6122
Last Date Modified: 10/28/2020

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

#include "ECE_Ghost.h"
#include "ECE_Bitmap.h"
#include "ECE_Pacman.h"
#include "MazeLevel.h"

#define ROW 22
#define COL 19


GLUquadricObj *quadratic2 = gluNewQuadric();

extern std::atomic<bool> inGame;
extern ECE_Pacman player;
extern mutex mutexTest;

bool thRunning;
auto lastLeft = std::chrono::high_resolution_clock::now();
auto startTime = std::chrono::high_resolution_clock::now();
//auto startTime = chrono::milliseconds(2000);
//int ECE_Ghost::iMazeData[2][2] = { (1, 1), (1, 1) };

//***************************************************************************
// Following 3 functions were to go with BFS (not implemented)
// https://www.geeksforgeeks.org/shortest-path-in-a-binary-maze/
// To store matrix cell cordinates
struct Point
{
	int x;
	int y;
};

// A Data Structure for queue used in BFS
struct queueNode
{
	Point pt;  // The cordinates of a cell
	int dist;  // cell's distance of from the source
};

// check whether given cell (row, col) is a valid
// cell or not.
bool isValid(int row, int col)
{
	// return true if row number and column number
	// is in range
	return (row >= 0) && (row < ROW) &&
		(col >= 0) && (col < COL);
}

//***************************************************************************
// TEXTURING FOR GHOST EYES
// texturing from example provided adjusted to only use one texture
GLuint texture[1];
struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char* data;
};
//create image struct with BMP type using Bitmap.h to load
typedef struct Image Image;
BMP inBitmap;
#define checkImageWidth 64
#define checkImageHeight 64
GLubyte checkImage[checkImageWidth][checkImageHeight][3];
//function to check the image dimensions and parse intensity
void makeCheckImage(void)
{
	int i, j, c;
	for (i = 0; i < checkImageWidth; i++) {
		for (j = 0; j < checkImageHeight; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
		}
	}
}
// read the image, generate textures included 4th dimension of alpha to use one map "eyes" is only to add eyes to our existing ghosts
void myinit(void)
{
	inBitmap.read("eyes_only.bmp");
	makeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Create Textures
	glGenTextures(1, texture);

	// Setup first texture
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 4, inBitmap.bmp_info_header.width, inBitmap.bmp_info_header.height, 0,
		GL_BGRA_EXT, GL_UNSIGNED_BYTE, &inBitmap.data[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);
}



//****************************************************************
// Friend function that each ghost thread runs on while in game
void threadFunc(ECE_Ghost* ptGhost)
//void pauseMove(ECE_Ghost* ptGhost)
{
	// for debugging
//	std::cout << "I'm a ghost!" << std::endl;
	bool startLoop = true;

	bool restartFlag = false;

	//if (inGame)
	if(true)
	{
		// waiting for player to press a key to start the ghosts
		while (!inGame)
		{
			startTime = std::chrono::high_resolution_clock::now(); // -std::chrono::milliseconds(5000);
		}
		// loops until the ghost has been released
		while (startLoop)
		{
			//		if ((chrono::high_resolution_clock::now() - lastLeft) > chrono::milliseconds(2000))
			if ((chrono::high_resolution_clock::now() - startTime) > ptGhost->waitTime) // -std::chrono::milliseconds(2000)) > ptGhost->waitTime)
			{
				ptGhost->exitHome();
				/*
//				std::cout << "X: " << ptGhost->location[0] << "  Y: " << ptGhost->location[1] << endl;
				ptGhost->isReleased = true;
				ptGhost->location[1] += 10;
//				std::cout << "X: " << ptGhost->location[0] << "  Y: " << ptGhost->location[1] << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(ptGhost->ghostSpeed));
				ptGhost->location[1] += 10;
//				std::cout << "I am released!" << endl;
//				std::cout << "X: " << ptGhost->location[0] << "  Y: " << ptGhost->location[1] << endl;

				// set invisible walls so ghosts can't go in, but remember to remove when the ghosts get reset
				ptGhost->pathMatrix[12][8] = 0;  // 12, and 8,9,10
				ptGhost->pathMatrix[12][9] = 0;
				ptGhost->pathMatrix[12][10] = 0;*/
				startLoop = false;
			}
		}
	}
	// infinite loop for thread to run in while game is running
	do
	{
		// while in game, run this loop
		do
		{
			// time to wait before making next move (how the ghosts move discretely)
			std::this_thread::sleep_for(std::chrono::milliseconds(ptGhost->ghostSpeed));
			ptGhost->ghostGamePlay();
//			mutexTest.lock();
			// triggered when the ghost gets eaten
			if (ptGhost->isAlive == false)
			{
				// time to wait before respawn
				std::this_thread::sleep_for(std::chrono::milliseconds(5000));
				ptGhost->respawn();
				ptGhost->exitHome();
			}
			// triggered when the player dies
			if (ptGhost->playerDeath == true)
			{
				ptGhost->respawn();
				std::this_thread::sleep_for(ptGhost->waitTime);
				ptGhost->exitHome();
				ptGhost->playerDeath = false;
			}

//			mutexTest.unlock();
		} while (inGame);
		// while not in game, run this loop (happens when pac-man dies a 4th time (used 3 lives)
		do
		{
			restartFlag = true;
			ptGhost->respawn();
			startTime = std::chrono::high_resolution_clock::now(); // -std::chrono::milliseconds(5000);
			
			do 
			{
				// once back in game:
				if (inGame && (chrono::high_resolution_clock::now() - startTime) > ptGhost->waitTime)
				{
					// triggered when the player dies
//					if (ptGhost->playerDeath == true)
//					{
						//					ptGhost->respawn();
						ptGhost->exitHome();
//						ptGhost->playerDeath = false;
//					}
					restartFlag = false;
				}
			} while (restartFlag);

		} while (!inGame);



	} while (true);
//	cout << "end of threadFunc";
};

//****************************************************************
// Determines whether to chase pac-man or scatter based on whether
//		ghost is vulnerable
void ECE_Ghost::ghostGamePlay()
{
//	while (true)
	{
		if (!isVulnerable)// && thisGhostColor == red)
		{
			// goal was to have 2 ghosts running with vector path and 2 using the BFS but I couldn't get BFS to work
			switch (thisGhostColor)
			{
			case red:
//				chaseByBFS();			// couldn't get it working :(
//				break;
			case green:
			case pink:
			case orange:
				chaseByVector();
				break;
			}
		}
		else if (isVulnerable)// && thisGhostColor == red)
		{
			scatter();
		}
	}
}

//****************************************************************
// Ghost constructor
ECE_Ghost::ECE_Ghost(ghostColor colorType)
{
	// what is happening:
	// since I declare the ghosts globally in main, they get constructed immediately, should move this timing logic into the main.cpp
//	if ( (chrono::high_resolution_clock::now() - lastLeft) > chrono::milliseconds(2500))
//	{
//		isReleased = true;
//	}
//	auto start = std::chrono::high_resolution_clock::now();
	thisGhostColor = colorType;
	switch (thisGhostColor)
	{
	case red:
		waitTime = std::chrono::milliseconds(0);
		ghostSpeed = std::chrono::milliseconds(300);		// 400
		xStart = 90;
		yStart = 120;
		break;
	case green:
		waitTime = std::chrono::milliseconds(2000);
		ghostSpeed = std::chrono::milliseconds(400);		// 600
		xStart = 80;
		yStart = 110;
		break;
	case pink:
		waitTime = std::chrono::milliseconds(4000);
		ghostSpeed = std::chrono::milliseconds(500);		// 800
		xStart = 90;
		yStart = 110;
		break;
	case orange:
		waitTime = std::chrono::milliseconds(6000);
		ghostSpeed = std::chrono::milliseconds(600);		// 1000
		xStart = 100;
		yStart = 110;
		break;
	}
//	if (isReleased == true)
	{
		mutexRelease.lock();
		startGhostThread = std::thread(threadFunc, this);
		mutexRelease.unlock();
		startGhostThread.detach();
	}
//	cout << "End of constructor";
}

//****************************************************************
// Reset location to starting position
void ECE_Ghost::resetLocation()
{
	this->location[0] = this->xStart;
	this->location[1] = this->yStart;
}

//****************************************************************
// Used to respawn the ghosts when it gets eaten or the player dies
void ECE_Ghost::respawn()
{
	// reset variables for the ghost when it respawns
	this->location[0] = this->xStart;
	this->location[1] = this->yStart;
	this->isAlive = true;
	this->isVulnerable = false;
	this->isReleased = true;
	this->playerDeath = false;

}

void ECE_Ghost::exitHome()
{
	// this is the best I could do to to get the ghosts to consisently leave the home-base and 
	//	and start chasing after pac-man but not go back in 
	this->pathMatrix[12][8] = 1;
	this->pathMatrix[12][9] = 1;
	this->pathMatrix[12][10] = 1;

	// move the ghosts straight out into the maze arena at their defined ghostSpeed
	//	std::cout << "Walls down!" << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(this->ghostSpeed));
	this->location[1] += 10;
	std::this_thread::sleep_for(std::chrono::milliseconds(this->ghostSpeed));
	this->location[1] += 10;
	//	std::cout << "I am respawned and released!" << endl;

	// set invisible walls so ghosts can't go in, but remember to remove when the ghosts get reset!
	this->pathMatrix[12][8] = 0;  // 12, and 8,9,10
	this->pathMatrix[12][9] = 0;
	this->pathMatrix[12][10] = 0;

	//	std::cout << "walls back up!" << endl;
}

//****************************************************************
// Default constructor
ECE_Ghost::ECE_Ghost()
{
	thisGhostColor = red;
}

//****************************************************************
// Function to find the shortest path between
//		a given source cell to a destination cell.
// I didn't get this working by time of submission, but the source is:
//	https://www.geeksforgeeks.org/shortest-path-in-a-binary-maze/
int BFS(int(*mat)[22][19], Point src, Point dest, ECE_Ghost* ptGhost)
{
	vector<queueNode> pathVector;

	// These arrays are used to get row and column
	// numbers of 4 neighbours of a given cell
	int rowNum[] = { -1, 0, 0, 1 };
	int colNum[] = { 0, -1, 1, 0 };

	int iii = 0;
	int jj = 0;
	// check source and destination cell
	// of the matrix have value 1
	if (!mat[src.x][src.y] || !mat[dest.x][dest.y])
		return -1;

	bool visited[ROW][COL];
	memset(visited, false, sizeof visited);

	// Mark the source cell as visited
	visited[src.x][src.y] = true;

	// Create a queue for BFS
	queue<queueNode> q;

	//map<queueNode, int> savedPath;
	//vector<queueNode> savedPathVector;

	// Distance of source cell is 0
	queueNode s = { src, 0 };
	q.push(s);  // Enqueue source cell

				// Do a BFS starting from source cell
	while (!q.empty())
	{
		queueNode curr = q.front();
		Point pt = curr.pt;

		if (size(q) == 1)
		{
			// not perfect but probably good enough for seeming to be finding pac-man from shortest path
//			cout << "only 1 in the queue" << endl;
			pathVector.push_back(q.front());
//			savedPathVector.push_back(q.front());
			//savedPath.insert(q, jj);
//			cout << "X: " << savedPathVector[jj].pt.x << "   Y: " << savedPathVector[jj].pt.y << "      Distance: " << savedPathVector[jj].dist << endl;
//			jj += 1;
		}

		// If we have reached the destination cell,
		// we are done
		if (pt.x == dest.x && pt.y == dest.y)
		{
			// bug - my original idea isn't working of collecting the values this way (only one element ends up existing in pathVector)
//			ptGhost->xNextBFS = (float)(pathVector[1].pt.y * 10);
//			ptGhost->yNextBFS = (float)(pathVector[1].pt.x * 10);
			return curr.dist;
		}

		// Otherwise dequeue the front 
		// cell in the queue
		// and enqueue its adjacent cells
		q.pop();

		for (int i = 0; i < 4; i++)
		{
			int row = pt.x + rowNum[i];
			int col = pt.y + colNum[i];

			// if adjacent cell is valid, has path and
			// not visited yet, enqueue it.
			if (isValid(row, col) && mat[row][col] &&
				!visited[row][col])
			{
				// mark cell as visited and enqueue it
				visited[row][col] = true;
				queueNode Adjcell = { { row, col },
					curr.dist + 1 };
				q.push(Adjcell);
				iii += 1;
				// Need to debug, got a memory debug assertion fail / abort  error
				// somewhere around 120-130 F5 jumps through debugger 
				if (iii > 123)
				{
					int dummy = 0;
				}

			}
		}
	}
	// Return -1 if destination cannot be reached
	return -1;
}

//****************************************************************
// Algorithm for ghosts to use to chase pac-man by BFS
void ECE_Ghost::chaseByBFS()
{
	// I didn't get this working by time of submission
	Point start;
	Point target;
	start.x = round((int)(this->location[0] / 10));
	start.y = round((int)(this->location[1] / 10));
	target.x = round((int)(player.location[0]/10));
	target.y = round((int)(player.location[1]/10));

	if (BFS(&pathMatrix, start, target, this) != -1)
	{
		// x, y are reversed from the BFS code
//		int xMove = this->pathVector[0].pt.y;
//		int yMove = this->pathVector[0].pt.x;

//		this->location[0] = start.x - xMove * 10;
//		this->location[1] = start.y - yMove * 10;
		this->location[0] = xNextBFS;
		this->location[1] = yNextBFS;

	}
}

//****************************************************************
// Algorithm for ghosts to use to chase pac-man by vector path
void ECE_Ghost::chaseByVector()//(float xPacMan, float yPacMan)
{
	bool keepDirection = true;
	bool foundMove = false;										// boolean to tell whether a valid move was found, used for randomly selecting direction until true
	//	bool validMove = true;										// boolean to tell whether there is a valid move available, used to determine whether to search
	std::default_random_engine generator(time(0));				//  generator(time(0)); for a new random each time
	std::uniform_int_distribution<int> distribution(1, 4);		// uniform distribution for generating the values 1-4 to determine direction
	int direction = 0;											// the int value of direction that's generated from the random generator and distribution
	//	int i = 0, j = 0;											// the value of the point on the grid that the ant is located
	bool upMove = false, downMove = false, leftMove = false, rightMove = false;		// booleans to tell whether there is a valid move per direction

	float xPacMan = player.location[0];
	float yPacMan = player.location[1];

	float xGhost = this->location[0];
	float yGhost = this->location[1];

	float dX = xPacMan - xGhost;
	float dY = yPacMan - yGhost;

	int xG = round((int)(location[0] / 10));
	int yG = round((int)(location[1] / 10));

	// check valid options for movement
	if (pathMatrix[yG][xG + 1] == 1)
	{
		rightMove = true;
	}
	if (pathMatrix[yG][xG - 1] == 1 )
	{
		leftMove = true;
	}
	if (pathMatrix[yG + 1][xG] == 1)
	{
		upMove = true;
	}
	if (pathMatrix[yG - 1][xG] == 1)
	{
		downMove = true;
	}

	// idea?? add some logic for like a known lastDirection bool and intersection bool, to keep ghost on same path until intersection

	// until a valid move is found
//	while (foundMove == false)
	{
	// if the absolute value of the X distance to pacman is greater than that of Y
		if (abs(dX) > abs(dY))
		{
			// move in the x-direction
			/*
			if (dX == 0 && dY > 0 && upMove)
			{
				this->location[1] += 10;
				foundMove = true;
				return;
			}
			if (dX == 0 && dY < 0 && downMove)
			{
				this->location[1] -= 10;
				foundMove = true;
				return;
			}*/

			if ((dX >= 0) && (xG + 1 < 19) && rightMove)
			{
				this->location[0] += 10;
				foundMove = true;
				return;
			}
			else if ((dX <= 0) && (xG - 1 > 0) && leftMove)
			{
				this->location[0] -= 10;
				foundMove = true;
				return;
			}
		}
		// if the absolute value of the Y distance to pacman is greater than that of X
		if (abs(dY) > abs(dX))
		{
			/*
			if (dY == 0 && dX > 0 && rightMove)
			{
				this->location[0] += 10;
				foundMove = true;
				return;
			}
			if (dY == 0 && dX < 0 && leftMove)
			{
				this->location[0] -= 10;
				foundMove = true;
				return;
			}*/

			// move in the y-direction
			if ((dY >= 0) && (yG + 1 < 22) && upMove)
			{
				this->location[1] += 10;
				foundMove = true;
				return;
			}
			else if ((dY <= 0) && (yG - 1 > 0) && downMove)
			{
				this->location[1] -= 10;
				foundMove = true;
				return;
			}
		}

		if (abs(dY) == abs(dX))
		{
			if (dX > dY)
			{
				if (rightMove)
				{
					this->location[0] += 10;
					foundMove = true;
					return;
				}
				if (downMove)
				{
					this->location[1] -= 10;
					foundMove = true;
					return;
				}
			}
			if (dX < dY)
			{
				if (leftMove)
				{
					this->location[0] -= 10;
					foundMove = true;
					return;
				}
				if (upMove)
				{
					this->location[1] += 10;
					foundMove = true;
					return;
				}
			}
			if (dX == dY && dX > 0)
			{
				if (rightMove)
				{
					this->location[0] += 10;
					foundMove = true;
					return;
				}
				if (upMove)
				{
					this->location[1] += 10;
					foundMove = true;
					return;
				}
			}
			if (dX == dY && dX < 0)
			{
				if (leftMove)
				{
					this->location[0] -= 10;
					foundMove = true;
					return;
				}
				if (downMove)
				{
					this->location[1] -= 10;
					foundMove = true;
					return;
				}
			}
		}



		if (dY == 0 && dX > 0 && rightMove)
		{
			this->location[0] += 10;
			foundMove = true;
			return;
		}
		if (dY == 0 && dX < 0 && leftMove)
		{
			this->location[0] -= 10;
			foundMove = true;
			return;
		}
		if (dX == 0 && dY > 0 && upMove)
		{
			this->location[1] += 10;
			foundMove = true;
			return;
		}
		if (dX == 0 && dY < 0 && downMove)
		{
			this->location[1] -= 10;
			foundMove = true;
			return;
		}


		// other wise choose a random direction if no valid move was found
		if (!foundMove)
		{
			while (foundMove == false)
			{
				// choose a random direction
				direction = distribution(generator);	// ...generate a random value for the direction until a valid move is found

				switch (direction)
				{
				case 1:
					if (upMove == true)
					{
						this->location[1] += 10;
						foundMove = true;
					}
					break;
				case 2:
					if (downMove == true)
					{
						this->location[1] -= 10;
						foundMove = true;
					}
					break;
				case 3:
					if (rightMove == true)
					{
						this->location[0] += 10;
						foundMove = true;
					}
					break;
				case 4:
					if (leftMove == true)
					{
						this->location[0] -= 10;
						foundMove = true;
					}
					break;
				}
			}
		}

		/*
		// same deal as scatter()
		while (keepDirection)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(this->ghostSpeed));
			xG = round((int)(location[0] / 10));
			yG = round((int)(location[1] / 10));

			// need a check for validity (bounds of grid) so code doesn't break

			// pathMatrix[y][x]
			// UP option, +Y
			if (yG + 1 < 22 && pathMatrix[yG + 1][xG] == 1)
			{
				upMove = true;
			}
			else
			{
				upMove = false;
			}
			// DOWN option, -Y
			if (yG - 1 >= 0 && pathMatrix[yG - 1][xG] == 1)
			{
				downMove = true;
			}
			else
			{
				downMove = false;
			}
			// LEFT option, -X
			if (xG - 1 >= 0 && pathMatrix[yG][xG - 1] == 1)
			{
				leftMove = true;
			}
			else
			{
				leftMove = false;
			}
			//RIGHT option, +X
			if (xG + 1 < 19 && pathMatrix[yG][xG + 1] == 1)
			{
				rightMove = true;
			}
			else
			{
				rightMove = false;
			}

			switch (direction)
			{
			case 1:			// UP, positive Y					// if the generated value is 1
				if (upMove == true)								// and up was a valid move
				{
					this->location[1] += 10;
				}
				else
				{
					keepDirection = false;
				}
				break;
			case 2:			// DOWN, negative Y					// if the generated value is 2
				if (downMove == true)							// and down was a valid move
				{
					this->location[1] -= 10;
				}
				else
				{
					keepDirection = false;
				}
				break;
			case 3:			// LEFT, negative X					// if the generated value is 3
				if (leftMove == true)							// and left was a valid move
				{
					this->location[0] -= 10;
				}
				else
				{
					keepDirection = false;
				}
				break;
			case 4:			// RIGHT, positive X				// if the generated value is 4
				if (rightMove == true)							// and right was a valid move
				{
					this->location[0] += 10;
				}
				else
				{
					keepDirection = false;
				}
				break;
			default:
				keepDirection = false;
				break;
			}
		}
		*/
	}
}

//****************************************************************
// Algorithm for ghost movement when they are vulnerable (white)
void ECE_Ghost::scatter()
{
	bool keepDirection = true;
	//	bool foundMove = false;										// boolean to tell whether a valid move was found, used for randomly selecting direction until true
	//	bool validMove = true;										// boolean to tell whether there is a valid move available, used to determine whether to search
	std::default_random_engine generator(time(0));				//  generator(time(0)); for a new random each time
	std::uniform_int_distribution<int> distribution(1, 4);		// uniform distribution for generating the values 1-4 to determine direction
	int direction = 0;											// the int value of direction that's generated from the random generator and distribution
	int i = 0, j = 0;											// the value of the point on the grid that the ant is located
	bool upMove = false, downMove = false, leftMove = false, rightMove = false;		// booleans to tell whether there is a valid move per direction
	int xG, yG;

	//while (isVulnerable)
	{
		xG = round( (int)(this->location[0] / 10) );
		yG = round( (int)(this->location[1] / 10) );

		// need a check for validity (bounds of grid) so code doesn't break
		
		// pathMatrix[y][x]
		// UP option, +Y
		if (yG+1 < 22 && this->pathMatrix[yG + 1][xG] == 1)
		{
			upMove = true;	
		}
		// DOWN option, -Y
		if (yG - 1 >= 0 && this->pathMatrix[yG - 1][xG] == 1)
		{
			downMove = true;
		}
		// LEFT option, -X
		if (xG - 1 >= 0 && this->pathMatrix[yG][xG - 1] == 1)
		{
			leftMove = true;
		}
		//RIGHT option, +X
		if (xG + 1 < 19 && this->pathMatrix[yG][xG + 1] == 1)
		{
			rightMove = true;
		}

		direction = distribution(generator);

		switch (direction)
		{
		case 1:			// UP, positive Y					// if the generated value is 1
			if (upMove == true)								// and up was a valid move
			{
				this->location[1] += 10;
			}
			break;
		case 2:			// DOWN, negative Y					// if the generated value is 2
			if (downMove == true)							// and down was a valid move
			{
				this->location[1] -= 10;
			}
			break;
		case 3:			// LEFT, negative X					// if the generated value is 3
			if (leftMove == true)							// and left was a valid move
			{
				this->location[0] -= 10;
			}
			break;
		case 4:			// RIGHT, positive X				// if the generated value is 4
			if (rightMove == true)							// and right was a valid move
			{
				this->location[0] += 10;
			}
			break;
		}

		while (keepDirection)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(this->ghostSpeed));
			xG = round((int)(this->location[0] / 10));
			yG = round((int)(this->location[1] / 10));

			// need a check for validity (bounds of grid) so code doesn't break

			// pathMatrix[y][x]
			// UP option, +Y
			if (yG + 1 < 22 && this->pathMatrix[yG + 1][xG] == 1)
			{
				upMove = true;
			}
			else
			{
				upMove = false;
			}
			// DOWN option, -Y
			if (yG - 1 >= 0 && this->pathMatrix[yG - 1][xG] == 1)
			{
				downMove = true;
			}
			else
			{
				downMove = false;
			}
			// LEFT option, -X
			if (xG - 1 >= 0 && this->pathMatrix[yG][xG - 1] == 1)
			{
				leftMove = true;
			}
			else
			{
				leftMove = false;
			}
			//RIGHT option, +X
			if (xG + 1 < 19 && this->pathMatrix[yG][xG + 1] == 1)
			{
				rightMove = true;
			}
			else
			{
				rightMove = false;
			}

			switch (direction)
			{
			case 1:			// UP, positive Y					// if the generated value is 1
				if (upMove == true)								// and up was a valid move
				{
					this->location[1] += 10;
				}
				else
				{
					keepDirection = false;
				}
				break;
			case 2:			// DOWN, negative Y					// if the generated value is 2
				if (downMove == true)							// and down was a valid move
				{
					this->location[1] -= 10;
				}
				else
				{
					keepDirection = false;
				}
				break;
			case 3:			// LEFT, negative X					// if the generated value is 3
				if (leftMove == true)							// and left was a valid move
				{
					this->location[0] -= 10;
				}
				else
				{
					keepDirection = false;
				}
				break;
			case 4:			// RIGHT, positive X				// if the generated value is 4
				if (rightMove == true)							// and right was a valid move
				{
					this->location[0] += 10;
				}
				else
				{
					keepDirection = false;
				}
				break;
			default:
				keepDirection = false;
				break;
			}
		}

	}
}

//****************************************************************
// converts matrix from MazeLevel to 0/1 matrix for pathfinding, where 1 = valid space, 0 = wall
void ECE_Ghost::setMazeData()
{
	for (int ii = 0; ii < 22; ++ii)		// from 0 to number of rows, y
	{
		for (int jj = 0; jj < 19; ++jj)	// from 0 to the number of columns, x
		{
			if (pathMatrix[ii][jj] != 0)
			{
				pathMatrix[ii][jj] = 1;		// 1 means the ghost can move in this space, while 0 is a wall
			}
		}
	}
}

//****************************************************************
// Renders a ghost with its set colors, and wraps eye texture
void ECE_Ghost::render(GLUquadricObj* quadr)
{
	// check the boolean of whether they're vulnerable or not and set normal vs. vulnerable colors
	if (!isVulnerable)
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambColor);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specColor);
	}
	else
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambWhiteGhost);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffWhiteGhost);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specWhiteGhost);
	}
	if (isAlive)
	{
		myinit();
		glPushMatrix();															// push to save where we're currently at
			glTranslatef(location[0], location[1], location[2]);				// translate to ghost's defined location
			glRotatef(0.0, 0.0, 1.0, 0.0);										// make perpendicular to xy plane
			gluCylinder(quadr, 5.0, 5.0, 5.0, 32, 32);							// draw a cylinder for body
		glPopMatrix();
		glPushMatrix();
			glTranslatef(location[0], location[1], 5.0f);
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glRotatef(-180.0f, -0.01f, 0.9f, 0.01f);
			gluQuadricTexture(quadratic2, GL_TRUE);
			gluSphere(quadratic2, 5.0, 30, 30);		
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();		
	}
}

void ECE_Ghost::setColor()
{
	// ghost colors, used as inputs to their color-setting class functions
	float colorRed[4] = { 0.9f, 0.0f, 0.1f, 1.0f };
	float ambRed[4] = { 0.9f, 0.0f, 0.1f, 1.0f };
	float diffRed[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; // {0.1, 0.1, 0.1, 1.0 }; //{ 0.55, 0.55, 0.55, 1.0};

	float colorGreen[4] = { 0.0f, 0.9f, 0.3f, 1.0f };
	float ambGreen[4] = { 0.0f, 0.9f, 0.3f, 1.0f };
	float diffGreen[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

	float colorPink[4] = { 1.0f, 0.4f, 0.85f, 1.0f };
	float ambPink[4] = { 1.0f, 0.4f, 0.85f, 1.0f };
	float diffPink[4] = { 1.0f, 0.5f, 0.8f, 1.0f };

	float colorOrange[4] = { 1.0f, 0.3f, 0.05f, 1.0f };
	float ambOrange[4] = { 1.0f, 0.3f, 0.05f, 1.0f };
	float diffOrange[4] = { 1.0f, 0.5f, 0.0f, 1.0f };

	switch (this->thisGhostColor)
	{
	case red:
		setAmbColor(ambRed);
		setDiffColor(diffRed);
		setSpecColor(colorRed);
		break;
	case green:
		setAmbColor(ambGreen);
		setDiffColor(diffGreen);
		setSpecColor(colorGreen);
		break;
	case pink:
		setAmbColor(ambPink);
		setDiffColor(diffPink);
		setSpecColor(colorPink);
		break;
	case orange:
		setAmbColor(ambOrange);
		setDiffColor(diffOrange);
		setSpecColor(colorOrange);
		break;
	}
}

//****************************************************************
// Sets the values of color[] array for the ghost, and sets the specular color for the ghost (same for all)
void ECE_Ghost::setSpecColor(float inColor[4])
{
	this->color2D[0] = inColor[0];
	this->color2D[1] = inColor[1];
	this->color2D[2] = inColor[2];
	this->color2D[3] = inColor[3];

	this->specColor[0] = 0.05f;
	this->specColor[1] = 0.05f;
	this->specColor[2] = 0.05f;
	this->specColor[3] = 0.05f;
}

//****************************************************************
// Sets the values of ambColor[] array for the ghost
void ECE_Ghost::setAmbColor(float inColor[4])
{
	this->ambColor[0] = inColor[0];
	this->ambColor[1] = inColor[1];
	this->ambColor[2] = inColor[2];
	this->ambColor[3] = inColor[3];
}

//****************************************************************
// Sets the values of diffColor[] array for the ghost
void ECE_Ghost::setDiffColor(float inColor[4])
{
	this->diffColor[0] = inColor[0];
	this->diffColor[1] = inColor[1];
	this->diffColor[2] = inColor[2];
	this->diffColor[3] = inColor[3];
}

//****************************************************************
// Set ghost's location in the maze with inputs
void ECE_Ghost::setLocation(float locationX, float locationY, float locationZ)
{
	this->location[0] = locationX;
	this->location[1] = locationY;
	this->location[2] = locationZ;
}


///////////////////////////////////////////////////////////////////////
//// NOT USED
void ECE_Ghost::wanderTimer()
{
	// call this when pac0man eats a power-up / when powerUpTime == true in main.cpp
	// but should this be a function in main?
	//	Sleep(5000);		// windows function, inaccurate, use chrono
	//	wanderMode = false;

}

void ECE_Ghost::getTargetLocation(float x, float y)
{
	targetLoc[0] = x;
	targetLoc[1] = y;
}

/*
void ECE_Ghost::setGhostColorType(ghostColor color)
{
//this->ghostColor() = color;
//ghostColor thisColor = color;
this->thisGhostColor = color;

}*/

//****************************************************************
// Set ghosts back to normal colors
void ECE_Ghost::ghostNormal()
{

}

//****************************************************************
// Set ghosts to white colors (would be called when pac-man eats power-up)
void ECE_Ghost::ghostWhite()
{

}
