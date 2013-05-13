#include <stdlib.h>
#include <stdio.h>

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
#include <GLUT/glut.h>
#endif
#endif



char* file_read(const char* filename);
void print_shader_log(GLuint object);
GLuint create_shader(const char* filename, GLenum type);