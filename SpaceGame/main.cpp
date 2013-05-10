//////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////

/* Include windows only for windows users :P
*/
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "windows.h"
#include <gl/glew.h>
#include "gl/GL.h"
#include <gl/freeglut.h>
#endif

#include <cassert>
#include <iostream>
#include "Mesh.h"
using namespace std;

int main(int _argc, char** _argv){
	cout<<"Hello, world!\n";
	cin.ignore(1);
	return 0;
}