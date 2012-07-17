// Wrapper class for glfw input functions

#include <glfw.h>

class Input {
	public:
		bool isKeyDown(int key);
		void getMousePos(int *xPos, int *yPos);
};