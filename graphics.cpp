// Wrapper class for OpenGL functions

#include <glfw.h>
#include "graphics.h"

void Graphics::rect(int x, int y, int w, int h)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(w, 0);
		glVertex2f(w, h); 
		glVertex2f(0, h);
	glEnd();
	glPopMatrix();
}

void Graphics::texturedRect(int x, int y, int w, int h, 
	double tx, double ty, double tw, double th)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(tx, ty + th);
		glVertex2f(0, 0);
		glTexCoord2f(tx + tw, ty + th);
		glVertex2f(w, 0);
		glTexCoord2f(tx+ tw, ty);
		glVertex2f(w, h); 
		glTexCoord2f(tx, ty);
		glVertex2f(0, h);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Graphics::line(int sx, int sy, int ex, int ey)
{
	glPushMatrix();
	glBegin(GL_LINE);
		glVertex2f(sx, sy);
		glVertex2f(ex, ey);
	glEnd();
	glPopMatrix();
}

void Graphics::setTexture(GLuint id)
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Graphics::loadTexture(char * imagepath)
{
	// Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
 
    // upload to texture memory
	glBindTexture(GL_TEXTURE_2D, textureID);
    glfwLoadTexture2D(imagepath, GLFW_BUILD_MIPMAPS_BIT);
 
    // set up trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void Graphics::setColour(float r, float g, float b)
{
	glColor3f(r, g, b);
}

void Graphics::flush(void)
{
	glfwSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}