/**
*Game State.h
*/
#ifndef GAME_STATE_H
#define GAME_STATE_H

#define USER_COMMAND_LOOK_UP (1)
#define USER_COMMAND_LOOK_RIGHT (2)
#define USER_COMMAND_LOOK_DOWN (3)
#define USER_COMMAND_LOOK_LEFT (4)
#define USER_COMMAND_MOVE_FORWARD (5)
#define USER_COMMAND_MOVE_BACKWARD (7)
#define USER_COMMAND_STRAFE_LEFT (6)
#define USER_COMMAND_STRAFE_RIGHT (8)
#define USER_COMMAND_FIRE_WEAPON (9)
#define USER_COMMAND_SWITCH_WEAPON (10)
#define USER_COMMAND_TOGGLE_ZOOM (11)
#define USER_COMMAND_USE_WEAPON (12)
#define USER_COMMAND_JUMP (13)
#define USER_COMMAND_INVENTORY (14)
#define USER_COMMAND_PAUSE_MENU (15)
#define USER_COMMAND_ACTION_OR_CONFIRM (16)
#define USER_COMMAND_FIRE_NAV_WEAPON (20)

//forward declare; 
class GameState;
class PSystems;

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
#else*/
#include <GL/glew.h>
#include <GLUT/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#endif

#include "GameRoom.h"
#include "GameObject.h"
//#include "Render.h" 
#include "Mesh.h"
#include "Camera.h" 
#include "GamePlayer.h"
#include <list>
#include "projectile_particles.h"
#include "UI.h" 
#include "AI.h"
#include "Monitor.h"
#include "Torch.h"

#define PLAYER_LOADOUT ("profileBegin.initP")
#define INITIAL_STATE_LOAD ("initialState.stamac")

class AI; 

/*
Particle Systems manager
*/
class PSystems {
	list<Projectile *> projectiles;
public: 
	//list<ParticleSystem> statics;
	Monitor monitor;  
	void updateAll(double dt);
	void AddBullet(Projectile *p){projectiles.push_back(p); }
	unsigned int NumBullets(){ return projectiles.size();}
	list<Projectile *> *GetBullets(){return &projectiles;} 
	Torch *t; 
};

/*
 *
 * Read in file for start state: include center of mass as a param...
 * Create reactionary physics motion
 * Collision Detection 
 * Debug and test
 *
 */



class GameState {
private:
	static GameState* m_pinstance;
	list<AI *> actors; 
	GameRoom* room; 
	Camera *cam;
	PSystems *ps;
	GameState(){
		ps = new PSystems(); paused=false; 
		Vector3f center(0, 0, 0); 
		Vector3f norm(0, 0, 1); 
		Vector3f up(0, 1, 0); 
		ps->t = new Torch(center,  norm, up, 1, 1, 5);   
	}
	float gravity;
	GameState(GameState const &stateMachine){}
	void operator=(GameState const &stateMachine){}
	GamePlayer player;
	void GameState::ProcessInput(list<UIEvent *> ev, double dt); 
	void GameState::AnimatePlayers(double dt);
public:
	static GameState* GameState::GetInstance();
	void GameState::AddProjectile(Projectile *p);
	void SetRoom(GameRoom *gr){this->room = gr;}
	void SetCamera(Camera *cam){this->cam =  cam;}
	void AddActor(AI *actor){actors.push_back(actor);} 
	list<AI *> *GetActors(){return &actors;} 
	GameRoom *GetRoom(){return this->room;}
	Camera *GetCamera(){return this->cam;} 
	Vector3f GetPlayerPosition(){return player.GetPosition();}
	void SetPlayerPosition(Vector3f pos){ player.SetPosition(pos);}
	bool GameState::ReadStateFile(const char *fname);
	void GameState::PerformStateActions(list<UIEvent *> ev, double dt);
	bool paused; 
	void UpdateParticleSystems(int dt /*ms*/){ps->updateAll(dt);}
	PSystems *GetParticleSystems(){return ps;} 
	bool GameState::ReadStateFile(const char *fname, GameState* sm, GameRoom &room);
	
}; 
#endif GAME_STATE_H
