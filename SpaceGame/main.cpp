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
#include "LocationDefines.h"
#include "Mesh.h"
#include "GameObjectHeaderList.h"
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
	
	//TaskQueue *main = new TaskQueue(NUM_THREADS);
	ConsoleCreateRoom();

	GameRoom gr; 	

	if (argc > 1){
		string path = pathCat(GAME_DATA_ROOMS_FOLDER, argv[1]);
		GameRoom::LoadRoom(path.c_str(), gr);
	}

	//For testing purposes.  To make sure it reads debug.room
	char debugName[1000];
	strcpy(debugName, GAME_DATA_ROOMS_FOLDER);
	strcat(debugName, "debug.room");
	GameRoom debug;
	assert(GameRoom::LoadRoom(debugName, debug));
	cout<<"Room name: "<<debug.GetName()<<endl;
	GameObject* object = debug.GetWorldObject("monkeyHead");
	assert(object != NULL);
	cout<<"Object name: "<<object->GetName()<<endl;
	cout<<"Object position: "<<object->GetPosition()<<endl;
	cout<<"Object rotation: "<<object->GetRotation()<<endl;
	cout<<"Object scale: "<<object->GetScale()<<endl;
	cout<<"Object meshFile: "<<object->GetMeshFile()<<endl;
	GameObject* light = debug.GetLight("ceiling");
	//To do: differentiate at the room level between object types....
	assert(light != NULL);
	cout<<"Light name: "<<light->GetName()<<endl;

	map<string, GameWorldObject>::iterator wobs = debug.GetRoomWorldObjectsIterator();
	while(wobs != debug.GetRoomWorldObjectsEnd()){
		cout<<(*wobs).second.GetName()<<endl;
		wobs++;
	}

	cin.ignore(1);
	///////////////////////////////////////////////////
	RenderGlutInitialize();


	/////////////////////////////////////////////////
	// TO DO:
	// Pass GameRoom debug to Render module.  Render the 
	// room.
	/////////////////////////////////////////////////

	glutMainLoop(); //this should only be called once, and AT THE END of the initialization routine.
	assert(debugger->CloseDebugFile());
	return 0;
}

