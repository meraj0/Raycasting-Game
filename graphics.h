// Wrapper class for OpenGL functions

#include <glfw.h>

class Graphics {
	public:
		void rect(int x, int y, int w, int h);
		void texturedRect(int x, int y, int w, int h, 
			double tx, double ty, double tw, double th);
		void line(int sx, int sy, int ex, int ey);
		void setTexture(GLuint id);
		void loadTexture(char * imagepath);
		void setColour(float r, float g, float b);
		void flush(void);
};