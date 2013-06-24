
#include "scottCollision.h"


void SCollision::performCollision(){
	GameRoom *gr = SCollision::gameState->GetRoom();
	//gr->monitor.Enter('r'); //What the hell's this for?
	vector<GameObject*> obs = gr->GetGameObjects();
	Camera *cam  = SCollision::gameState->GetCamera();

	list<Projectile *> *bullets = SCollision::gameState->GetParticleSystems()->GetBullets();  	
	list<Projectile *>::iterator it = bullets->begin(); 
	while(it != bullets->end()){  
		Projectile *curBullet = *it;

	}


	//gr->monitor.Exit('r'); //seriously, what's this for?
}