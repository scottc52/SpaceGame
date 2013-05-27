// Basic template for event-driven objects: doors and switches.

#ifndef _GAMEEVENTOBJECT_H_
#define _GAMEEVENTOBJECT_H_

#include "GameActiveObject.h"
#include "GameState.h"

class GameEventObject : public GameActiveObject {
public:
	GameEventObject(GameState *gameState) {
		isActivated = false;
		this->gameState = gameState;
	}
	virtual void ActivateIfConditionsMet() = 0;
	virtual void Activate() = 0;
protected: 
	GameState *gameState;
	bool isActivated;
};

#endif