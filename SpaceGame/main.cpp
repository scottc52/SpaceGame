//////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////

#ifdef _WIN32
/* Include windows only for windows users :P
*/
#include "windows.h"
#endif
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
	glutInit(&argc, argv);
	GameDebugger* debugger = GameDebugger::GetInstance();
	assert(debugger->OpenDebugFile());
	debugger->WriteDebugMessageToConsole("Hello, World", 31);
	debugger->WriteToDebugFile("Wrote to file", 32);
	ConsoleCreateRoom();

	RenderGlutInitialize();

	glutMainLoop(); //this should only be called one, and AT THE END of the initialization routine.
	assert(debugger->CloseDebugFile());
	return 0;
}

