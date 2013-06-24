/**
* controller.cpp
*/
#include "Controller.h"
#include <pthread.h>
#include <sched.h>

Controller *Controller::gameController = NULL;
Sound* Controller::sound = NULL;

double avg = FPS; 
#define WEIGHT ((double)(1.0/(5.0 * (double)FPS)))
void Controller::run(){
	while (1){
		double delta = GameTime::DiffTimeMS(ref);		 
 		while (delta < 4.0){ // so we don't kill cpu
 			//cout << "too fast! (in a good way)" << endl;
 			pthread_helper_sleep(1);
 			delta = GameTime::DiffTimeMS(ref);  
 		}
 		GameTime::GameTimer nextRef = GameTime::GetTime();
 		
		list<UIEvent *> events; 
 		PCInputManager::AllPending(events); 
	//delta = GameTime::DiffTimeMS(t);
	//cout<<"it took: " << delta << "ms to process events" << endl; 
		delta = GameTime::DiffTimeMS(ref);	
		state->PerformStateActions(events, delta); 


	//scott : add player movement and add collision detection 
	movePlayer();
	SCollision::performCollision();

		
	//render here
	//Render::myDisplay(); 
		double since_frame = GameTime::DiffTimeMS(lastFrame);	
		if ( since_frame > MSPF - 2){
			if(Render::requestFrame()){
				lastFrame = GameTime::GetTime();
				double afr = 1.0/(since_frame / 1000.0);
				double effw = (since_frame / (double) MSPF) * WEIGHT;  
				avg = (1-effw) * avg + afr * effw ;
				//cout << "FPS: " << afr << "5-Sec AVG: " << avg << endl;  
			}
		}

		double frame_time = GameTime::DiffTimeMS(ref);
		if( ((double)(rand() % 10000)) / 1000.0 < 0.01) {
			//cerr << "updates took" << (frame_time - 5.0) << endl; 
		}
	
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


//move player hack
void Controller::movePlayer(){
		GameState * gameState = state;
		if(NavShot::isPlayerMoving){
			float speed = 0.1;
			Camera *cam  = gameState->GetCamera();
			Vector3f pos = cam->getPivotPoint();//gameState->GetPlayerPosition();
			Vector3f newPos = Vector3f(0.0, 0.0, 0.0);
			if((NavShot::targetPlayerPos - pos).norm() < speed){
				newPos = NavShot::targetPlayerPos*1.0;
				NavShot::isPlayerMoving = false;
					if (sound) {
						sound->Play();
					}else{
						sound = new Sound("sounds/impact.wav");
						sound->Play();
					}
				//cin.ignore(1);
			}else{
				Vector3f dir = (NavShot::targetPlayerPos - pos).normalized();
				newPos = pos+dir*speed;
				//cout << "first\n" << pos << "\n";\
				//cout << "new\n" << newPos << "\n";
				//cin.ignore(1);
			}
			//cout << "\n\n\nhmmmmmm\n\n\n" ;
			gameState->SetPlayerPosition(newPos);
			cam->setPivotPoint(newPos);
		}
	}