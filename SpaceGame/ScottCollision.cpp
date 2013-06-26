
#include "scottCollision.h"


bool SCollision::paused = false;
Sound *SCollision::sound = NULL;
GameState *SCollision::gameState = NULL;


void SCollision::performCollision(){
	float playerHitRadius = 0.2;
	if(!sound){ sound = new Sound("sounds/clang.wav"); }

	GameRoom *gr = SCollision::gameState->GetRoom();
	//gr->monitor.Enter('r'); //What the hell's this for?
	vector<GameObject*> obs = gr->GetGameObjects();
	Camera *cam  = SCollision::gameState->GetCamera();

	list<Projectile *> *bullets = SCollision::gameState->GetParticleSystems()->GetBullets();  	
	list<Projectile *>::iterator it1 = bullets->begin(); 
	while(it1 != bullets->end()){  
		Projectile *p1 = *it1;
		if(p1->isDead()){it1++; continue;}
		if(p1->typeString == "Enemy"){
			//calculate collision between enemy and bullets
			list<Projectile *>::iterator it2 = bullets->begin();
			while(it2 != bullets->end()){ 
				Projectile *p2 = *it2;
				if(p2->isDead()){it2++; continue;}
				if(p2->owner == "player" && p2->typeString != "NavShot"){ //enemy should take damage at collision
					if( (p1->getPosition() - p2->getPosition()).norm() < (p1->hitRadius+p2->hitRadius)){
						//enemy p1 takes damage -> use hit() to code them
						sound->Play();
						p1->health = p1->health - p2->damage;
						p2->hit(p2->getPosition());
						p1->hit(p1->getPosition());
						if(p1->isDead()){
							//create and attach explosion where enemy was
							Projectile *pNew = new SmokyBullet(p1->getPosition(),
								Vector3f(0.0,0.0,0.0), 0.8, 0.1, 0.1, 0.7);
							pNew->hit(p1->getPosition());
							gameState->AddProjectile(pNew);
						}
					}
				}
				it2++;
			}
		}
		//calculate collision between player and all other projectiles
		if(p1->owner != "player" && p1->typeString != "NavShot"){
			if( (cam->getPosition() - p1->getPosition()).norm() < (playerHitRadius+p1->hitRadius)){
				//player takes damage
				sound->Play();
				Render::health = Render::health - p1->damage;
				Render::hitEffect();
				p1->hit(p1->getPosition());
			}
		}


		//process firing bullets
		if(p1->firesOwnBullets && p1->fireBulletCountDown <= 0.0){  //fire a bullet towards cam
			p1->fireBulletCountDown = 20000.0 * (rand()%100000 / 100000.0) ;;
			Vector3f shootDir = (cam->getPosition() - p1->getPosition());
			Vector3f delta =  Vector3f( ( (rand()%100000+1) / 100000.0)-0.5 , 
										( (rand()%100000+1) / 100000.0)-0.5 , 
										( (rand()%100000+1) / 100000.0)-0.5  );
			shootDir = shootDir + delta*0.3;
			shootDir = shootDir.normalized()*2;
			Projectile *pNew = new Slug(p1->getPosition(),
				shootDir, 0.8, 0.1, 0.1, 0.7);
			pNew->owner = "enemy";
			pNew->damage = 5.0;
			//pNew->hit(p1->getPosition());
			gameState->AddProjectile(pNew);
		}

		it1++; 
	}

	
	//deleteDeadEnemies from repel list
	ProjectileEnemies::deleteDead();


	//gr->monitor.Exit('r'); //seriously, what's this for?
}