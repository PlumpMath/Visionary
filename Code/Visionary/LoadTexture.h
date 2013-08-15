
#include <windows.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

bool LoadTextureIntoGL( GLuint &iTexture, BYTE* data, GLuint width, GLuint height, GLuint depth, GLuint size, GLuint type, int wrap );