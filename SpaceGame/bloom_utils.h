#ifndef _BLOOMUTILS_H_
#define _BLOOMUTILS_H_
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#ifdef _WIN32
#include <gl/glew.h>
#include "gl/GL.h"
#include <GL/glu.h>
#include <gl/freeglut.h>
#else
#ifdef __linux__
#include <gl/glew.h> 
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#else
#include <GL/glew.h>
#include <GLUT/glut.h>
#endif
#endif

using namespace std;

struct Viewport{
    GLint x;
    GLint y;
    GLsizei width;
	GLsizei height;
};

struct Surface{
	GLsizei width;
    GLsizei height;
    struct Viewport viewport;
    GLuint texture;
    GLuint depth;
    GLuint fbo;
	GLboolean useDepth;
	GLboolean useFp; 
};

void createSurface(Surface *, GLboolean depth, GLboolean fp, GLboolean linear, GLboolean clampToEdge);
void bindSurface(Surface *);
void clearSurfaceColor(GLfloat c0, GLfloat c1, GLfloat c2, GLfloat c3);
void resizeSurface(Surface *surface, int w, int h);
#endif _BLOOMUTILS_H_