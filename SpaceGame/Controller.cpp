/**
* controller.cpp
*/
#include "Controller.h"

Controller *Controller::gameController = NULL;

#define STATS 
#undef STATS

void Controller::run(){
	double delta = GameTime::DiffTimeMS(ref);		 
 	GameTime::GameTimer t;// = GameTime::GetTime();  
 	list<UIEvent *> events; 
 	PCInputManager::AllPending(events); 
	//delta = GameTime::DiffTimeMS(t);
	//cout<<"it took: " << delta << "ms to process events" << endl; 
	
	state->PerformStateActions(events, dt); 
	//AI updates

	//animate here

	//update particles
	t = GameTime::GetTime(); 
	state->UpdateParticleSystems(dt);
	delta = GameTime::DiffTimeMS(t);
	int len = state->GetParticleSystems()->GetBullets()->size();  	
	//cout<<"it took: " << delta << "ms to update (" << len << ") bullets" << endl; 
	
	//render here
	//Render::myDisplay(); 		
	Render::requestFrame();
	double frame_time = GameTime::DiffTimeMS(ref);
	
	//cout << "it took (but not really): " << frame_time << endl; 
	
	int DroppedFrames = frame_time / MSPF;
	int time_out = MSPF- frame_time; 
	if (DroppedFrames){
		time_out = 0;		
	} 

	glutTimerFunc(time_out, Controller::GlutSync, (int)time_out + frame_time); 
}

void Controller::Initialize(TaskQueue *tq, GameState *gs){
	Controller *gc = new Controller(tq, gs);
	gameController = gc;   
}

void Controller::GlutSync(int dt){
	GAME_DEBUG_ASSERT(gameController != NULL);
	gameController->ref = GameTime::GetTime(); 
	gameController->dt = dt;		
	gameController->taskManager->enqueue(gameController);  
}
