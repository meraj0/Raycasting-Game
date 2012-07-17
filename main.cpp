#include <string>
#include <vector>

#include "graphics.h"
#include "input.h"
#include "display.h"

const unsigned short WIDTH = 1280;
const unsigned short HEIGHT = (WIDTH / 4) * 3;

Graphics graphics; 
Input input;

double playerX = 22;
double playerY = 12;

double dirX = -1;
double dirY = 0;

double planeX = 0;
double planeY = 0.66;

const double moveSpeed = 0.1; // squares per second
const double rotSpeed = 0.1; // radians per second

#define mapWidth 26
#define mapHeight 26

char worldMap[mapWidth][mapHeight]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,3,0,3,0,3,0,0,0,0,1},
  {1,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,3,0,3,0,3,0,0,0,0,1},
  {1,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,2,2,0,2,2,0,0,0,0,0,0,3,0,3,0,3,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,4,4,0,0,4,4,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,4,0,4,4,0,4,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void pollInput(void);
void renderWalls(void);
void renderBackground(void);

int main(int argc, char * argv)
{
	Display display(WIDTH, HEIGHT, false, "Raycasting Game");

	graphics.loadTexture("red.tga"); // these have to be loaded in the correct order
    graphics.loadTexture("green.tga"); // the order corresponds to the numbers on the map
    graphics.loadTexture("blue.tga");
    graphics.loadTexture("yellow.tga");

	bool running = true;

	while (running) 
	{
		pollInput();

		renderBackground();
		renderWalls();

		graphics.flush();

		running = !input.isKeyDown(GLFW_KEY_ESC) && !display.isCloseRequested();
	}

	display.destroy();
	exit(0);
}

void pollInput(void)
{
	//move forward if there isn't a wall
	if (input.isKeyDown(GLFW_KEY_UP) || input.isKeyDown('W'))
	{
		if(worldMap[int(playerX + dirX * moveSpeed)][int(playerY)] == false) playerX += dirX * moveSpeed;
		if(worldMap[int(playerX)][int(playerY + dirY * moveSpeed)] == false) playerY += dirY * moveSpeed;
	}
	//move backwards if there isn't a wall
	if (input.isKeyDown(GLFW_KEY_DOWN) || input.isKeyDown('S'))
	{
		if(worldMap[int(playerX - dirX * moveSpeed)][int(playerY)] == false) playerX -= dirX * moveSpeed;
		if(worldMap[int(playerX)][int(playerY - dirY * moveSpeed)] == false) playerY -= dirY * moveSpeed;
	}
	//rotate clockwise
	if (input.isKeyDown(GLFW_KEY_RIGHT) || input.isKeyDown('D'))
	{
		//both direction and camera plane must be rotated
		double oldDirX = dirX;
		dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
		dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
		double oldPlaneX = planeX;
		planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
		planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
	}
	//rotate anticlockwise
	if (input.isKeyDown(GLFW_KEY_LEFT) || input.isKeyDown('A'))
	{
		//both direction and camera plane must be rotated
		double oldDirX = dirX;
		dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
		dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
		double oldPlaneX = planeX;
		planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
		planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
	}
}

void renderWalls(void)
{
	for(int x = 0; x < WIDTH; x++)
	{
		double cameraX = 2 * x / double(WIDTH) - 1; // x-coordinate in camera space
		double rayPosX = playerX;
		double rayPosY = playerY;
		double rayDirX = dirX + planeX * cameraX;
		double rayDirY = dirY + planeY * cameraX;

		// which box of the map we're in  
		int mapX = int(rayPosX);
		int mapY = int(rayPosY);
       
		// length of ray from current position to next x or y-side
		double sideDistX;
		double sideDistY;
       
		// length of ray from one x or y-side to next x or y-side
		double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
		double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
		double perpWallDist;
       
		// what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;

		bool hit = false; // was there a wall hit?
		char side; // was a NS or a EW wall hit?

		// calculate step and initial side distance
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (rayPosX - mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
		}

		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (rayPosY - mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
		}
		// perform differential analysis
		while (!hit)
		{
			// jump to next map square, OR in x-direction, OR in y-direction
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}

			if (worldMap[mapX][mapY] > 0) hit = true;
		} 
		// calculate distance projected on camera direction to stop the fisheye effect
		if (side == 0)
			perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
		else
			perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
      
		int lineHeight = abs(int(HEIGHT / perpWallDist));
       
		// calculate lowest and highest pixel to fill in current wall slice
		int startY = -lineHeight / 2 + HEIGHT / 2;
		if(startY < 0)startY = 0;
		int endY = lineHeight / 2 + HEIGHT / 2;
		if(endY >= HEIGHT)endY = HEIGHT - 1;

		// calculate the correct texture offset and colours for the current wall slice
		double textureX;

		if (side == 1) 
		{
			graphics.setColour(0.75f, 0.75f, 0.75f);
			textureX = rayPosX + ((mapY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
		}
		else
		{
			graphics.setColour(0.5f, 0.5f, 0.5f);
			textureX = rayPosY + ((mapX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
		}
		textureX -= floor((textureX));

		// draw it!
        graphics.setTexture(worldMap[mapX][mapY]);
		graphics.texturedRect(x, startY, 1, endY - startY, textureX, 0.0, 1 / 64, 1.0);
	}
}

void renderBackground(void)
{
	// ceiling
	int r;
	for (r = 0; r < HEIGHT / 2; r += 10)
	{
		graphics.setColour(0.2f, 0.2f, 0.2f);
		graphics.rect(0, r, WIDTH, 10);
	}
	// ground
	for (; r < HEIGHT; r += 15) 
	{
		graphics.setColour(0.1f, 0.1f, 0.1f);
		graphics.rect(0, r, WIDTH, 15);
	}
}
