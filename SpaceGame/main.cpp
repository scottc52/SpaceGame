//////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////

#ifdef _WIN32
/* Include windows only for windows users :P
*/
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "windows.h"
#endif
#include <gl/glew.h>
#include "gl/GL.h"
#include <GL/glu.h>
#include <gl/freeglut.h>
#endif

#include <cassert>
#include <iostream>
#include "Mesh.h"
#include "GameDebug.h"
#include "GameWorldObject.h"
#include "GameCamera.h"
#include "GameDoor.h"
#include "GameRoom.h"
#include "GameItem.h"
#include "GameLight.h"
#include "Render.h"
#include "RoomBuilder.h"
#include "TaskQueue.h"
#include "LocationDefines.h"
#include "TextIOHelpers.h"

#define NUM_THREADS ((unsigned int )8)

//****************************************************
// Main function, initialize program here
//****************************************************
int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	GameDebugger* debugger = GameDebugger::GetInstance();
	assert(debugger->OpenDebugFile());
	debugger->WriteDebugMessageToConsole("Hello, World", 31);
	debugger->WriteToDebugFile("Wrote to file", 32);
	
	TaskQueue *main = new TaskQueue(NUM_THREADS);
	ConsoleCreateRoom();

	GameRoom gr; 	

	if (argc > 1){
		string path = pathCat(GAME_DATA_ROOMS_FOLDER, argv[1]);
		GameRoom::LoadRoom(path.c_str(), gr);
	}
	
	
	RenderGlutInitialize();

	glutMainLoop(); //this should only be called one, and AT THE END of the initialization routine.
	assert(debugger->CloseDebugFile());
	return 0;
}

