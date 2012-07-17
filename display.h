// Wrapper class for glfw functions

#include <glfw.h>

class Display {
	public:
		Display(short width, short height, bool resizable, char * title);
		bool isCloseRequested(void);
		void destroy(void);
};