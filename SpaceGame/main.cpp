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

/**
legacy callback handlers
#define STEP ((float)0.5)

void move(GameState *s, Vector3f &delta){
	Camera *c = s->GetCamera(); 
	Vector3f tmp = c->getPivotPoint();
	tmp += delta; 
	c->setPivotPoint(tmp);
	cout << "position x: " << tmp.x()
		<< " y: " << tmp.y()
		<< " z: " << tmp.z()
		<< endl; 
}

void moveCBQ(GameState *s){
	Vector3f delta(0, -STEP, 0);
	move(s, delta);  
}

void moveCBE(GameState *s){
	Vector3f delta(0, STEP, 0);
	move(s, delta); 
} 

void moveCBA(GameState *s){
	Vector3f delta(-STEP, 0, 0);
	move(s, delta);  
} 

void moveCBD(GameState *s){
	Vector3f delta(STEP, 0, 0);
	move(s, delta);  
}  

void moveCBW(GameState *s){
	Vector3f delta(0, 0, STEP);
	move(s, delta);
} 

void moveCBS(GameState *s){
	Vector3f delta(0, 0, -STEP);
	move(s, delta);  
} 

void fireCB(GameState *s){
	s->FireBullet(); 
}

void hitCB(GameState *s){
	Render::hitEffect();
}

void killCB(GameState *s){
	exit(0);
}
int X = -1; int Y = -1; 
double theta = 0; 

#include <cmath> 

//x = cos(theta)
//x = sin(theta)

void mouseMoveCB(int x, int y){
	int dx =0, dy = 0; 
	if (X > 0){
		dx = x -X; 
	} 
	if (Y > 0){
		dy = y -Y;
	}
	//map add a sensitivity
	theta += ((double)dx*2) * 3.14159 / 180.0; 

	float ly = 0;
	float lx = cos(theta);
	float lz = sin(theta); 

	Vector3f dir(lx, ly, lz); 

	GameState::GetInstance()->GetCamera()->setDirection(dir); 

	Y = y;
	X = x; 
}

void mouseFunc(int button, int state, int x, int y){
	if (button==GLUT_LEFT_BUTTON && state==GLUT_UP){
		X=-1;
		Y=-1;
	}
}
*/
/**
Define Behaviors for the game. This is here so that people can see them easily. We can move it into a particular module if
that seems more appropriate.  
*/


void RegisterControls(){
	PCInputManager *controls = new PCInputManager();
	controls->setKeyCallback(0, '\t', true, USER_COMMAND_PAUSE_MENU);
	controls->setKeyCallback(0, ((unsigned char )13), true, USER_COMMAND_ACTION_OR_CONFIRM);
	controls->setKeyCallback(0, 'e', true, USER_COMMAND_USE_WEAPON);
	controls->setKeyCallback(0, 'a', true, USER_COMMAND_STRAFE_LEFT);
	controls->setKeyCallback(0, 'd', true, USER_COMMAND_STRAFE_RIGHT);
	controls->setKeyCallback(0, 's', true, USER_COMMAND_MOVE_BACKWARD);
	controls->setKeyCallback(0, 'w', true, USER_COMMAND_MOVE_FORWARD);
	controls->setKeyCallback(0, ' ', true, USER_COMMAND_JUMP);
	controls->setKeyCallback(0, 'q', true, USER_COMMAND_SWITCH_WEAPON);
	controls->setMouseButtonCallback(0, UI_LEFT_BUTTON, true, USER_COMMAND_FIRE_WEAPON); 
	controls->setMouseButtonCallback(0, UI_MIDDLE_BUTTON, true, USER_COMMAND_TOGGLE_ZOOM); 
	controls->setMouseMoveCallback(0, false, USER_COMMAND_LOOK_UP);
	controls->setActiveCommandSet();
	PCInputManager::EnableUI();   
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
			string fname = pathCat(".", gwo->GetMeshFile()); 
			if (! MyMeshIO::LoadMesh(*tmp, fname)){
				cerr<<"couldn't load (" << gwo->GetMeshFile() << ") for " <<gwo -> GetName() <<endl;  
				gwo->SetMesh(NULL);
			}else{
				gwo->SetMesh(tmp);
			}
		} 
		wobs++;
		
	}

	cin.ignore(1);
	///////////////////////////////////////////////////
	
	//TODO: load from file	
	GameState *gs = GameState::GetInstance(); 
	Vector3f pos(0.0f, 0, -10.0f); 
	Vector3f up(0, 1.0f, 0); 
	Vector3f dir(0.0f, 0, 1.0f); 
	float radius =0, n = 0.1, f = 600, fovy = 80, aspect = ((float)16.0/9.0); 
	Camera cam(pos, dir, up, radius, n, f, fovy, aspect); 
	gs->SetRoom(&debug);
	gs->SetCamera(&cam);
	Render::GlutInitialize();
	Render::gameState = gs;
	
	RegisterControls(); 
	Controller::Initialize(taskManager, gs); 	
	
	
	/////////////////////////////////////////////////
	// TO DO:
	// Pass GameRoom debug to Render module.  Render the 
	// room.
	/////////////////////////////////////////////////
	glutTimerFunc(0, Controller::GlutSync, 0);
	//glutMotionFunc(mouseMoveCB);
	//glutMouseFunc(mouseFunc);
	glutMainLoop(); //this should only be called once, and AT THE END of the initialization routine.
	assert(debugger->CloseDebugFile());
	return 0;
}

