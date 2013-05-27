/**
*Game State.h
*/
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameRoom.h"
#include "GameObject.h"
#include "Render.h" 
#include "Camera.h"

//TODO
/*
 *
 * Read in file for start state: include center of mass as a param...
 * Create reactionary physics motion
 * Collision Detection 
 * Debug and test
 *
 */

#define INITIAL_STATE_LOAD ("initialState.stamac")

class GameState{
private:
	GameState(){}
	static GameState* m_pinstance;
	float gravity;
	GameState(GameState const &stateMachine){};
	void operator=(GameState const &stateMachine){};
public:
	static GameState* GameState::GetInstance();
	bool GameState::ReadStateFile(const char *fname, GameState* sm, GameRoom &room);
	void GameState::UpdateState(GameRoom &room/*, GameInput input*/);
}; 

#endif GAME_STATE_H
