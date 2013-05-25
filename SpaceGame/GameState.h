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
	GameState(){}
	static GameState* m_pinstance;
	float gravity;
	GameState(GameState const &stateMachine){};
	void operator=(GameState const &stateMachine){};
public:
	static GameState* GameState::GetInstance();
	//bool GameState::ReadStateFile();
	//void GameState::UpdateState(GameRoom &room/*, GameInput input*/);
}; 

#endif GAME_STATE_H
