/**
* controller.cpp
*/
#include "Controller.h"

Controller *Controller::gameController = NULL;

#define STATS 
#undef STATS

void Controller::run(){
	while (1){
		double delta = GameTime::DiffTimeMS(ref);		 
 		while (delta < 5.0){ // so we don't kill cpu
 			//cout << "too fast! (in a good way)" << endl;
 			pthread_yield(); 
 			delta = GameTime::DiffTimeMS(ref);  
 		}
 		GameTime::GameTimer nextRef = GameTime::GetTime();
 		GameTime::GameTimer t;// = GameTime::GetTime();  
 		list<UIEvent *> events; 
 		PCInputManager::AllPending(events); 
	//delta = GameTime::DiffTimeMS(t);
	//cout<<"it took: " << delta << "ms to process events" << endl; 
		delta = GameTime::DiffTimeMS(ref);	
		state->PerformStateActions(events, delta); 
	
	//render here
	//Render::myDisplay(); 		
		if (GameTime::DiffTimeMS(lastFrame) > MSPF -1){
			if(Render::requestFrame())
				lastFrame = GameTime::GetTime();
		}
		double frame_time = GameTime::DiffTimeMS(ref);
		if( ((double)(rand() % 10000)) / 1000.0 < 0.01)
			cerr << "updates took" << (frame_time - 5.0) << endl; 
	
		//cout << "it took (but not really): " << frame_time << endl; 
		//if (DroppedFrames){
		//	time_out = 0;		
		//} 

		ref = nextRef; 
	}
}

void Controller::Initialize(TaskQueue *tq, GameState *gs){
	Controller *gc = new Controller(tq, gs);
	gameController = gc;   
}

void Controller::GlutSync(int dt){
	GAME_DEBUG_ASSERT(gameController != NULL);
	gameController->ref = GameTime::GetTime();
	gameController->lastFrame = GameTime::GetTime();  
	gameController->dt = dt;		
	gameController->taskManager->enqueue(gameController);  
}
