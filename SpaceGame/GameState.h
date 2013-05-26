/**
*Game State.h
*/
#ifndef GAME_STATE_H
#define GAME_STATE_H

//forward declare; 
class GameState;
class PSystems; 

#include "GameRoom.h"
#include "GameObject.h"
#include "Render.h" 
#include "Camera.h" 
#include <list>
#include "projectile_particles.h"

typedef class SmokyBullet SmokeyBullet; 

class PSystems{
	list<SmokyBullet *> projectiles; // add a super class to generalize if we add more types
public: 
	//list<ParticleSystem> statics; 
	void updateAll(int dt){
		list<SmokeyBullet *>::iterator it = projectiles.begin();
		while(it != projectiles.end()){
			SmokeyBullet *sb = *it;			
			if(sb->isDead()){
				it = projectiles.erase(it);
				delete (sb);
			}else{ 			
				//hitboxing;
				if(sb->t > 4000 && !sb->hitB){
					sb->hit(sb->location);
				} 
				sb->update(dt);
				it ++; 
			}  
		} 
	}
	list<SmokyBullet *> *GetBullets(){return &projectiles;} 
};

class GameState{
private:
	GameRoom *room; 
	Camera *cam;
	PSystems *ps; 
public:
	GameState(){ps = new PSystems();} 
	GameRoom *GetRoom(){return room;}
	Camera *GetCamera(){return cam;}
	void SetRoom(GameRoom *gr){this->room = gr;}
	void SetCamera(Camera *cam){this->cam =  cam;} 
	Vec3f GetPlayerPosition(){return Vec3f(0, 0, 0);}

	//Action API --> update state based of event handlers
	//see main.cpp for examples
	void FireBullet(){//fire bullet!
		Vector3f loc = Vector3f(0, -1, 4); //below camera
		Vector3f vel = Vector3f(0.0, 0.0, -4.8);
		Vector4f col = Vector4f(0.5, 0.0, 0.45, 0.5);
		SmokyBullet *bullet = new SmokyBullet(loc, vel, col[0], col[1], col[2], col[3]);
		ps->GetBullets()->push_back(bullet);}
	void UpdateParticleSystems(int dt /*ms*/){ps->updateAll(dt);}
	PSystems *GetParticleSystems(){return ps;} 
}; 

#endif
