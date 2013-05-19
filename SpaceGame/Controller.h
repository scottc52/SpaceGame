/**
*Controller.h
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
#include "ctime.h"

#define FPS (60) 
#define MSPF () 

Controller *gameController = NULL; 

/**
*
*/ 
class Controller{
private: 
	TaskQueue* taskManager;
	GameState* state; 
	RenderHandle *rh;  
public: 
	
	void init(TaskQueue *taskPool):taskManager(taskPool){
		
	}

	void run(){
		
		//update state here

		//animate here

		//render here
	}

	static void glutSync(){
		GAME_DEBUG_ASSERT(gameController != NULL);
		gameController->run();
	}
};

#endif
