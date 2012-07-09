#define _USE_MATH_DEFINES
#define FLOAT_MAX 9999999

#include <glfw.h>
#include <stdlib.h>
#include <math.h>

const unsigned short WIDTH = 960;
const unsigned short HEIGHT = (WIDTH / 4) * 3;

const unsigned short TILE_SIZE = 64;
const unsigned short WALL_HEIGHT = 128;

const unsigned short ANGLE60 = WIDTH;
const unsigned short ANGLE30 = (ANGLE60 / 2);
const unsigned short ANGLE15 = (ANGLE30 / 2);
const unsigned short ANGLE90 = (ANGLE30 * 3);
const unsigned short ANGLE180 = (ANGLE90 * 2);
const unsigned short ANGLE270 = (ANGLE90 * 3);
const unsigned short ANGLE360 = (ANGLE60 * 6);
const unsigned short ANGLE0 = 0;
const unsigned short ANGLE5 = (ANGLE30 / 6);
const unsigned short ANGLE1 = (ANGLE5 / 5);
const unsigned short ANGLE10 = (ANGLE5 * 2);

float sinTable [ANGLE360 + 1];
float iSinTable [ANGLE360 + 1];
float cosTable [ANGLE360 + 1];
float iCosTable [ANGLE360 + 1];
float tanTable [ANGLE360 + 1];
float iTanTable [ANGLE360 + 1];
float fishTable [ANGLE60 + 1];
float xStepTable [ANGLE360 + 1];
float yStepTable [ANGLE360 + 1];

unsigned short playerX = 292;
unsigned short playerY = 160;
int playerArc = ANGLE0;
unsigned short int playerDistanceToTheProjectionPlane = 277;
unsigned short int playerSpeed = 3;
unsigned short int projectionPlaneYCenter = HEIGHT / 2;

const unsigned short MAP_WIDTH = 12;
const unsigned short MAP_HEIGHT = 12;

const char W = 1;
const char O = 0;

char map [MAP_WIDTH * MAP_HEIGHT] =
		{
				W, W, W, W, W, W, W, W, W, W, W, W,
				W, O, O, O, O, O, O, O, O, O, O, W,
				W, O, W, O, O, O, O, O, O, W, O, W,
				W, O, O, W, O, O, O, O, W, O, O, W,
				W, O, O, W, O, O, O, O, W, O, O, W,
				W, O, O, W, W, O, O, W, W, O, O, W,
				W, O, O, W, W, O, O, W, W, O, O, W,
				W, O, O, W, O, O, O, O, W, O, O, W,
				W, O, O, W, O, O, O, O, W, O, O, W,
				W, O, W, O, O, O, O, O, O, W, O, W,
				W, O, O, O, O, O, O, O, O, O, O, W,
				W, W, W, W, W, W, W, W, W, W, W, W
		};

float arcToRad(float arcAngle);
void createTables(void);
void fillRect(int x, int y, int w, int h);
void tick(void);
void drawBackground(void);
void render(void);
void quit(int return_code);

int main(void)
{

	if (glfwInit() != GL_TRUE)
		exit(1);

	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, true);

	if (glfwOpenWindow(WIDTH, HEIGHT, 5, 6, 5, 0, 0, 0, GLFW_WINDOW) != GL_TRUE)
		exit(1);

	glfwSetWindowTitle("Raycasting Game");
	
	// create the tables for speedy trig
	createTables();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIDTH, HEIGHT, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);

	while (true) 
	{
		// tick the game logic
		tick();
		// clear the buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw the figure
		render();
		// swap back and front buffers
		glfwSwapBuffers();
	}

}

float arcToRad(float arcAngle)
{
	return ((float) (arcAngle * M_PI) / (float) ANGLE180);
}

void createTables(void)
{
	int i;
	float radian;

	for (i = 0; i <= ANGLE360; i++) 
	{
		// get the radian value (the last addition is to avoid division by 0 because that would
		// result in a hole in the wall when a ray is at 0, 90, 180, or 270)
		radian = arcToRad(i) + (float) (0.0001);
		sinTable[i] = (float) sin(radian);
		iSinTable[i] = (1.0F / (sinTable[i]));
		cosTable[i] = (float) cos(radian);
		iCosTable[i] = (1.0F / (cosTable[i]));
		tanTable[i] = (float) tan(radian);
		iTanTable[i] = (1.0F / tanTable[i]);

		//  you can see that the distance between xi is the same
		//  if we know the angle
		//  _____|_/next xi______________
		//       |
		//  ____/|next xi_________   slope = tan = height / dist between xi's
		//     / |
		//  __/__|_________  dist between xi = height/tan where height=tile size
		// old xi|
		//                  distance between xi = x_step[view_angle];
		//
		// facing left
		if (i >= ANGLE90 && i < ANGLE270) 
		{
			xStepTable[i] = (float) (TILE_SIZE / tanTable[i]);
			if (xStepTable[i] > 0)
				xStepTable[i] = -xStepTable[i];
		}
		// facing right
		else 
		{
			xStepTable[i] = (float) (TILE_SIZE / tanTable[i]);
			if (xStepTable[i] < 0)
				xStepTable[i] = -xStepTable[i];
		}

		// facing down
		if (i >= ANGLE0 && i < ANGLE180) 
		{
			yStepTable[i] = (float) (TILE_SIZE * tanTable[i]);
			if (yStepTable[i] < 0)
				yStepTable[i] = -yStepTable[i];
		}
		// facing up
		else 
		{
			yStepTable[i] = (float) (TILE_SIZE * tanTable[i]);
			if (yStepTable[i] > 0)
				yStepTable[i] = -yStepTable[i];
		}
	}

	for (i = -ANGLE30; i <= ANGLE30; i++) 
	{
		radian = arcToRad(i);
		// we don't have negative angle, so make it start at 0
		// this will give range 0 to 320
		fishTable[i + ANGLE30] = (float) (1.0F / cos(radian));
	}
}

void tick(void)
{
	// store the old positions so we can revert to them later on in case
	// of a collision
	int oldX = playerX;
	int oldY = playerY;

	// rotate left
	if (glfwGetKey('A') == GLFW_PRESS || glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if ((playerArc -= ANGLE1) < ANGLE0)
			playerArc += ANGLE360;
	}
	// rotate right
	else if (glfwGetKey('D') == GLFW_PRESS || glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) 
	{
		if ((playerArc += ANGLE1) >= ANGLE360)
			playerArc -= ANGLE360;
	}

	//  _____     _
	// |\ arc     |
	// |  \       y
	// |    \     |
	//            -
	// |--x--|
	//
	//  sin(arc)=y/diagonal
	//  cos(arc)=x/diagonal   where diagonal=speed
	float playerXDir = cosTable[playerArc];
	float playerYDir = sinTable[playerArc];

	// move forward
	if (glfwGetKey('W') == GLFW_PRESS || glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) 
	{
		playerX += (int) (playerXDir * playerSpeed);
		playerY += (int) (playerYDir * playerSpeed);
	}
	// move backward
	else if (glfwGetKey('S') == GLFW_PRESS || glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS) 
	{
		playerX -= (int) (playerXDir * playerSpeed);
		playerY -= (int) (playerYDir * playerSpeed);
	}

	// check if we have collided with a wall
	if(map[MAP_WIDTH * (playerY / 64) + (playerX / 64)] == W) 
	{
		playerX = oldX;
		playerY = oldY;
	}

	if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
	{
		quit(0);
	}
}

void render(void)
{
	drawBackground();

	int verticalGrid;        // horizotal or vertical coordinate of intersection
	int horizontalGrid;      // theoritically, this will be multiple of TILE_SIZE

	int distToNextVerticalGrid; // how far to the next bound (this is multiple of
	int distToNextHorizontalGrid; // tile size)

	float xIntersection;
	float yIntersection;

	float distToNextXIntersection;
	float distToNextYIntersection;

	int xGridIndex;  // the current cell that the ray is in
	int yGridIndex;

	float distToVerticalGridBeingHit;      // the distance of the x and y ray intersections from
	float distToHorizontalGridBeingHit;      // the viewpoint

	int castArc, castColumn;

	castArc = playerArc;
	// field of view is 60 degree with the point of view (player's direction in the middle)
	// 30  30
	//    ^
	//  \ | /
	//   \|/
	//    v
	// we will cast the rays starting from the left
	castArc -= ANGLE30;
	// wrap around if necessary
	if (castArc < 0) {
		castArc = ANGLE360 + castArc;
	}

	for (castColumn = 0; castColumn < WIDTH; castColumn += 5) 
	{

		// ------------------------------
		//	FIND HORIZONTAL INTERSECTIONS
		// ------------------------------

		// is the ray facing down?
		if (castArc > ANGLE0 && castArc < ANGLE180) 
		{
			// truncuate then add to get the coordinate of the FIRST grid (horizontal
			// wall) that is in front of the player
			horizontalGrid = (playerY / TILE_SIZE) * TILE_SIZE + TILE_SIZE;

			// distance to the next horizontal wall
			distToNextHorizontalGrid = TILE_SIZE;

			float xtemp = iTanTable[castArc] * (horizontalGrid - playerY);

			// find the x interception to that wall
			xIntersection = xtemp + playerX;
		}
		// the ray must be facing up
		else 
		{
			horizontalGrid = (playerY / TILE_SIZE) * TILE_SIZE;
			distToNextHorizontalGrid = -TILE_SIZE;

			float xtemp = iTanTable[castArc] * (horizontalGrid - playerY);
			xIntersection = xtemp + playerX;

			horizontalGrid--;
		}
		// look for a horizontal wall
		if (castArc == ANGLE0 || castArc == ANGLE180) 
		{
			distToHorizontalGridBeingHit = FLOAT_MAX;	//if the ray is at 0 or 180 it's not going to collide
		}
		// move the ray until it hits a horizontal wall
		else 
		{
			distToNextXIntersection = xStepTable[castArc];
			while (true) 
			{
				xGridIndex = (int) (xIntersection / TILE_SIZE);
				// in the picture, yGridIndex will be 1
				yGridIndex = (horizontalGrid / TILE_SIZE);
				if ((xGridIndex >= MAP_WIDTH) ||
						(yGridIndex >= MAP_HEIGHT) ||
						xGridIndex < 0 || yGridIndex < 0) 
				{
					distToHorizontalGridBeingHit = FLOAT_MAX;
					break;
				} else if ((map[yGridIndex * MAP_WIDTH + xGridIndex]) != O)
				{
					distToHorizontalGridBeingHit = (xIntersection - playerX) * iCosTable[castArc];
					break;
				}
				// else, the ray is not blocked, extend to the next block
				else
				{
					xIntersection += distToNextXIntersection;
					horizontalGrid += distToNextHorizontalGrid;
				}
			}
		}

		// ----------------------------
		//	FIND VERTICAL INTERSECTIONS
		// ----------------------------

		// is the ray facing right?
		if (castArc < ANGLE90 || castArc > ANGLE270) 
		{
			verticalGrid = TILE_SIZE + (playerX / TILE_SIZE) * TILE_SIZE;
			distToNextVerticalGrid = TILE_SIZE;

			float ytemp = tanTable[castArc] * (verticalGrid - playerX);
			yIntersection = ytemp + playerY;
		}
		// the ray must be facing left
		else
		{
			verticalGrid = (playerX / TILE_SIZE) * TILE_SIZE;
			distToNextVerticalGrid = -TILE_SIZE;

			float ytemp = tanTable[castArc] * (verticalGrid - playerX);
			yIntersection = ytemp + playerY;

			verticalGrid--;
		}
		
		if (castArc == ANGLE90 || castArc == ANGLE270) 
		{
			distToVerticalGridBeingHit = FLOAT_MAX; // if the ray is at 90 or 270 it's not going to collide
		} 
		else 
		{
			distToNextYIntersection = yStepTable[castArc];
			while (true) 
			{
				xGridIndex = (verticalGrid / TILE_SIZE);
				yGridIndex = (int) (yIntersection / TILE_SIZE);

				if ((xGridIndex >= MAP_WIDTH) ||
						(yGridIndex >= MAP_HEIGHT) ||
						xGridIndex < 0 || yGridIndex < 0) 
				{
					distToVerticalGridBeingHit = FLOAT_MAX;
					break;
				} else if ((map[yGridIndex * MAP_WIDTH + xGridIndex]) != O) 
				{
					distToVerticalGridBeingHit = (yIntersection - playerY) * iSinTable[castArc];
					break;
				} else 
				{
					yIntersection += distToNextYIntersection;
					verticalGrid += distToNextVerticalGrid;
				}
			}
		}

		// -------------------------------------
		//	DRAW THE WALLS THAT HAVE BEEN FOUND
		// -------------------------------------

		float dist;
		int topOfWall;
		int bottomOfWall;

		if (distToHorizontalGridBeingHit < distToVerticalGridBeingHit) 
		{
			dist = distToHorizontalGridBeingHit;
			glColor3f(0.75f, 0.75f, 0.75f);
		}
		else 
		{
			dist = distToVerticalGridBeingHit;
			glColor3f(0.5f, 0.5f, 0.5f);
		}

		// compensate for the fishbowl effect
		dist /= fishTable[castColumn];

		int projectedWallHeight = (int) (WALL_HEIGHT * (float) playerDistanceToTheProjectionPlane / dist);
		bottomOfWall = projectionPlaneYCenter + (int) (projectedWallHeight * 0.5F);
		topOfWall = HEIGHT - bottomOfWall;

		if (bottomOfWall >= HEIGHT)
			bottomOfWall = HEIGHT - 1;

		fillRect(castColumn, topOfWall, 5, projectedWallHeight);

		// set up to cast the next ray
		castArc += 5;
		if (castArc >= ANGLE360)
			castArc -= ANGLE360;
	}
} 

void drawBackground(void)
{
	// ceiling
	int r;
	for (r = 0; r < HEIGHT / 2; r += 10)
	{
		glColor3f(0.0f, 0.1f, 0.1f);
		fillRect(0, r, WIDTH, 10);
	}
	// ground
	for (; r < HEIGHT; r += 15) 
	{
		glColor3f(0.0f, 0.1f, 0.1f);
		fillRect(0, r, WIDTH, 15);
	}
}

void fillRect(int x, int y, int w, int h)
{
	glBegin(GL_QUADS);
		glVertex2i(x, y);
		glVertex2i(x + w, y);
		glVertex2i(x + w, y + h);
		glVertex2i(x, y + h);
	glEnd();
}

void quit(int return_code)
{
  glfwTerminate();
  exit(return_code);
}