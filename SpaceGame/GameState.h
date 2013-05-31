/**
*Game State.h
*/
#ifndef GAME_STATE_H
#define GAME_STATE_H


///////////////////////USER COMMAND 
#define USER_COMMAND_LOOK_UP (1)
#define USER_COMMAND_LOOK_DOWN (3)
#define USER_COMMAND_LOOK_LEFT (2)
#define USER_COMMAND_LOOK_RIGHT (4)
#define USER_COMMAND_MOVE_FORWARD (5)
#define USER_COMMAND_MOVE_BACKWARD(7)
#define USER_COMMAND_STRAFE_LEFT (6)
#define USER_COMMAND_STRAFE_RIGHT(8)
#define USER_COMMAND_FIRE_WEAPON (9)
#define USER_COMMAND_SWITCH_WEAPON (10)
#define USER_COMMAND_TOGGLE_ZOOM (11)
#define USER_COMMAND_USE_WEAPON (12)
#define USER_COMMAND_JUMP (13)
#define USER_COMMAND_INVENTORY (14)
#define USER_COMMAND_PAUSE_MENU (15)
#define USER_COMMAND_ACTION_OR_CONFIRM (16)



//forward declare; 
class GameState;
class PSystems; 

#include "GameRoom.h"
#include "GameObject.h"
#include "Render.h" 
#include "Camera.h" 
#include "GamePlayer.h"
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

/*
 *
 * Read in file for start state: include center of mass as a param...
 * Create reactionary physics motion
 * Collision Detection 
 * Debug and test
 *
 */

#define PLAYER_LOADOUT ("profileBegin.initP")
#define INITIAL_STATE_LOAD ("initialState.stamac")

class GameState{
private:
	static GameState* m_pinstance;
	GameRoom* room; 
	Camera *cam;
	PSystems *ps;
	GameState(){ps = new PSystems();}
	float gravity;
	GameState(GameState const &stateMachine){}
	void operator=(GameState const &stateMachine){}
	GamePlayer player;
public:
	static GameState* GameState::GetInstance();
	void SetRoom(GameRoom *gr){this->room = gr;}
	void SetCamera(Camera *cam){this->cam =  cam;}
	GameRoom *GetRoom(){return this->room;}
	Camera *GetCamera(){return this->cam;} 
	Vec3f GetPlayerPosition(){return Vec3f(0, 0, 0);}
	bool GameState::ReadStateFile(const char *fname);
	void GameState::PerformStateActions();

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
	bool GameState::ReadStateFile(const char *fname, GameState* sm, GameRoom &room);
	
}; 
#endif GAME_STATE_H
