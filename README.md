# ECE6122-FinalProject-3DPacMan

	Final Project for ECE6122 - 3D Pac-Man game built utilizing OpenGL and multithreading

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
	|		|
	|    (maze)	|
	|		|
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

    
