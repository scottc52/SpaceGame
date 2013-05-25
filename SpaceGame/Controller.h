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
#include "UI.h"
#include <ctime>

#define FPS (60) 
#define MSPF ((double)(1.0 / ((double) FPS) * 1000.0))  



using namespace std; 

double diffTimeMS(clock_t ref){
	clock_t cur = clock();
	clock_t delta = ref- cur; 
	double ms = (double)delta / ((double)(CLOCKS_PER_SEC)) * 1000.0; 	
	return ms;	 
}

/**
*
*/ 
class Controller : public Task{
private: 
	TaskQueue* taskManager;
	GameState* state; 
	//RenderHandle *rh;
	static Controller *gameController;  
	clock_t ref;
	int dt;   
	Controller(TaskQueue *tq, GameState *gs): taskManager(tq), state(gs){}
public:
	/* 
	polls events -- updates state 
	main loop for drawing a frame -- updates state
	*/
	virtual void run(){
		cout << "Update Frame" << endl; 
		//Poll events and update state 
	 	PCInputManager::ExecutePendingEvents(); 
		
		//update particles 
		state->updateParticleSystems(dt);  	
		//animate here

		//render here
		//Render::myDisplay(); 		
		glutPostRedisplay();
		
		double frame_time = diffTimeMS(ref);
		cout << "it took (but not really): " << frame_time << endl; 
		int DroppedFrames = frame_time / MSPF;
		int time_out = MSPF - frame_time; 
		if (DroppedFrames){
			time_out = 0;		
		}  
		glutTimerFunc(time_out, Controller::GlutSync, (int)time_out + frame_time); 
	}
	
	static void Initialize(TaskQueue *tq, GameState *gs){
		Controller *gc = new Controller(tq, gs);
		gameController = gc;   
	}

	static void GlutSync(int dt){
		GAME_DEBUG_ASSERT(gameController != NULL);
		gameController->ref = clock(); 
		gameController->dt = dt;		
		gameController->taskManager->enqueue(gameController);  
	}
};

Controller *Controller::gameController = NULL;

#endif
