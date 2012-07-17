// Wrapper class for GLFW display functions

#include <glfw.h>
#include "display.h"

Display::Display(short width, short height, bool resizable, char *title)
{
	if (glfwInit() != GL_TRUE)
		return;

	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, !resizable);

	if (glfwOpenWindow(width, height, 5, 6, 5, 0, 0, 0, GLFW_WINDOW) != GL_TRUE)
		return;

	glfwSetWindowTitle(title);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
}

bool Display::isCloseRequested(void)
{
	return !glfwGetWindowParam(GLFW_OPENED);
}

void Display::destroy(void)
{
	glfwTerminate();
}

