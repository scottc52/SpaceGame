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
#include "GameState.h"
#include "Render.h"
#include "UI.h"
#include <ctime>
#include "GameTime.h"
#include "pthread_helpers.h" 

#include "ScottCollision.h"

#define FPS (60) 
#define MSPF ((double)(1.0 / ((double) FPS) * 1000.0))  



using namespace std; 


/**
*
*/ 
class Controller : public Task{
private: 
	TaskQueue* taskManager;
	GameState* state; 
	//RenderHandle *rh;
	static Controller *gameController;  
	GameTime::GameTimer ref;
	GameTime::GameTimer lastFrame; 
	double dt;   
	Controller(TaskQueue *tq, GameState *gs): taskManager(tq), state(gs){}
	static Sound *sound;
public:
	/* 
	polls events -- updates state -- updates geometry -- returns control to render
	*/
	virtual void run();

	void movePlayer();
	
	static void Initialize(TaskQueue *tq, GameState *gs);//{}

	static void GlutSync(int dt);
};

#endif
