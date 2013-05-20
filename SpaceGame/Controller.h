/**
*Controller.h The game controller handles the tie-ins with the windowing system
* (glut) and handles synchronizing the calls to the various models in the gameplay pipeline  
*/ 

#ifndef CONTROLLER_H
#define CONTROLLER_H


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

#include "TaskQueue.h"
#include "GameDebug.h"
#include <ctime>

#define FPS (60) 
#define MSPF ((double)(1.0 / ((GLFloat) FPS) * 1000.0))  



using namespace std; 

Controller *gameController = NULL; 

double diffTime(clock_t ref){
	clock_t cur = clock() ; 
}

/**
*
*/ 
class Controller{
private: 
	TaskQueue* taskManager;
	GameState* state; 
	RenderHandle *rh;  
public: 
	
	Contoller(TaskQueue *taskPool):taskManager(taskPool){
		//TODO: logic for picking controller scheme
		state=new GameState(); 
		PCInputManager::EnableUI(*state);
		RenderGlutInitialize();

	}

	void run(){
		
		//update state here

		//animate here

		//render here
	}

	static Initialize(TaskQueue tq){
		GameController *gc = new GameController(tq);
		gameController = gc; 
	}

	static void glutSync(int framesDropped){
		GAME_DEBUG_ASSERT(gameController != NULL);
		clock_t ref = clock();               
		gameController->run();
		double frame_time = difftime(ref); 
		int DroppedFrames = frame_time / MSPF;
		int time_out = MSPF - frame_time; 
		if (DroppedFrames){
			time_out = 0;		
		}  
		glutTimerFunc((int)MSPF - frame_time, Controller::glutSync, droppedFrames); 
	}
};

#endif
