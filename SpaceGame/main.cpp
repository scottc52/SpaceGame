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
#include "Controller.h"
#include "UI.h"

#define NUM_THREADS ((unsigned int )8)

/*if (key == 'p' || key == 'P') {
		//hit effect
		lastHit = glutGet(GLUT_ELAPSED_TIME);
	}
	else if (key == 'o' || key == 'O') {
		//fire bullet!
		Vector3f loc = Vector3f(0, -1, 4); //below camera
		Vector3f vel = Vector3f(0.0, 0.0, -4.8);
		Vector4f col = Vector4f(0.95, 0.0, 0.0, 0.5);
		bullet = new SmokyBullet(loc, vel, col[0], col[1], col[2], col[3]);
	}
	else if (key == 'q' || key == 'Q') exit(0);
*/
void fireCB(GameState *s){
	s->FireBullet(); 
}

void killCB(GameState *s){
	exit(0);
}

/*void foggifyCB(GameState *s){ 
	s->
}*/ 

/**
Define Behaviors for the game. This is here so that people can see them easily. We can move it into a particular module if
that seems more appropriate.  
*/
void mapCallbacksPC(){
	PCInputManager *controls = new PCInputManager();
	controls->setKeyCallback(0, 'o', true, fireCB);
	controls->setKeyCallback(0, 'q', true, killCB);
	controls->setActiveCommandSet();  
}

//****************************************************
// Main function, initialize program here
//****************************************************
int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	GameDebugger* debugger = GameDebugger::GetInstance();
	assert(debugger->OpenDebugFile());
	debugger->WriteDebugMessageToConsole("Hello, World", 31);
	debugger->WriteToDebugFile("Wrote to file", 32);
	
	TaskQueue *taskManager = new TaskQueue(NUM_THREADS);
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
	cout<<"Object scale: "<< object->GetScale()<<endl;
	cout<<"Object meshFile: "<< object->GetMeshFile()<<endl;
	GameObject* light = debug.GetLight("ceiling");
	//To do: differentiate at the room level between object types....
	assert(light != NULL);
	cout<<"Light name: "<<light->GetName()<<endl;

	map<string, GameWorldObject>::iterator wobs = debug.GetRoomWorldObjectsIterator();
	while(wobs != debug.GetRoomWorldObjectsEnd()){
		//load starting meshes
		GameWorldObject *gwo = &(wobs->second);
		cout<<gwo->GetName()<<endl;
		MyMesh *tmp = new MyMesh();
		if (gwo->GetMeshFile()){
			if (! IO::read_mesh(*tmp, gwo->GetMeshFile(), 0)){
				cerr<<"couldn't load (" << gwo->GetMeshFile() << ") for " <<gwo -> GetName() <<endl;  
			}else{
				gwo->SetMesh(tmp);
			} 
		} 
		wobs++;
		
	}

	cin.ignore(1);
	///////////////////////////////////////////////////
	
	//TODO: load from file	
	GameState *gs = new GameState(); 
	gs->SetRoom(&debug);
		
	Render::GlutInitialize();
	Render::gameState = gs;

	PCInputManager::EnableUI(gs);	
	mapCallbacksPC(); 
	Controller::Initialize(taskManager, gs); 	
	
	
	/////////////////////////////////////////////////
	// TO DO:
	// Pass GameRoom debug to Render module.  Render the 
	// room.
	/////////////////////////////////////////////////
	glutTimerFunc(0, Controller::GlutSync, 0);
	glutMainLoop(); //this should only be called once, and AT THE END of the initialization routine.
	assert(debugger->CloseDebugFile());
	return 0;
}

