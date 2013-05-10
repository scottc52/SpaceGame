//////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////

/* Include windows only for windows users :P
*/
#include "windows.h"
#include <gl/glew.h>
#include "gl/GL.h"
#include <GL/glu.h>
#include <gl/freeglut.h>
#include <cassert>
#include <iostream>
#include "Mesh.h"
#include "GameDebug.h"
#include "GameWorldObject.h"
#include "GameCamera.h"
#include "GameDoor.h"
#include "GameItem.h"
#include "GameLight.h"
#include "Render.h"
#include "RoomBuilder.h"

//****************************************************
// Main function, initialize program here
//****************************************************
int main(int argc, char *argv[]){
	int err = GameDebugInitialize();
	assert(err == 0);
	ConsoleCreateRoom();

	int c = argc;
	char** v = argv;
	RenderGlutInitialize(c, v);

	err = GameDebugFinish();
	assert(err == 0);
	cin.ignore(1);
	return 0;
}

