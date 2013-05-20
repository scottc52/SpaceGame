/**
*Game State.h
*/
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameRoom.h"
#include "GameObject.h"
#include "Render.h" 
#include "Camera.h"

class GameState{
private:
	GameRoom *room; 
	Camera *cam;
public:
	GameRoom *GetRoom(){return room;}
	Camera *GetCamera(){return cam;}
	Vec3f GetPlayerPosition(){return Vec3f(0, 0, 0);} 
}; 

#endif
