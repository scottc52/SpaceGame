/*
 * Active (movable/destructible) object for game.
 * Placed in the world.  State machine operates on these things.
 */
#ifndef _GAMEACTIVEOBJECT_H_
#define _GAMEACTIVEOBJECT_H_
#include "GameObject.h"

#define GAMESTATE_NEUTRAL ("neutral")
#define GAMESTATE_UNACTIVATED ("unactivated")
#define GAMESTATE_ACTIVATED ("activated")
#define GAMESTATE_LOCKED ("locked")
#define GAMESTATE_UNLOCKED ("unlocked")


class GameActiveObject : public GameObject{
private:
	Vec3f CenterOfMass;
	string state;
public:
	GameActiveObject::GameActiveObject(){
		state = "neutral";
	}

	void GameActiveObject::SetState(const char* s){state = s;}
	const char* GameActiveObject::GetState(){return state.c_str();}
};
#endif _GAMEACTIVEOBJECT_H_