// Basic template for event-driven objects: doors and switches.

#ifndef _GAMEEVENTOBJECT_H_
#define _GAMEEVENTOBJECT_H_

#include "GameActiveObject.h"
class GameState;

class GameEventObject : public GameActiveObject {
public:
	GameEventObject(GameState *gameState);
	GameEventObject();
	virtual void ActivateIfConditionsMet() = 0;
	virtual void Activate() = 0;
protected: 
	GameState *gameState;
	bool isActivated;
};

#endif