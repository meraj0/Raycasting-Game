// Wrapper class for GLFW input functions

#include <glfw.h>
#include "input.h"

bool Input::isKeyDown(int key)
{
	return glfwGetKey(key) == GLFW_PRESS;
}

void Input::getMousePos(int *xPos, int *yPos)
{
	return glfwGetMousePos(xPos, yPos);
}