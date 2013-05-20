/**
*Game State.h
*/
#ifndef GAME_STATE_H
#define GAME_STATE_H



#include "GameRoom.h"
#include "GameObject.h"
#include "Render.h" 

class GameState{
private:
	GameRoom *room; 
public:
	GameRoom *GetRoom();
	Vec3f GetPlayerPosition(); 
	
}; 

#endif