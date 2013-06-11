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
	string state;
	//Rotation information
public:
	GameActiveObject::GameActiveObject(){
		//velocity = Vec3f(0,0,0);
		state = "neutral";
		objType = ACTIVE_OBJECT_TYPE;
	}

	void GameActiveObject::SetState(const char* s){state = s;}
	const char* GameActiveObject::GetState(){return state.c_str();}
	Vector3f GameActiveObject::CenterOfMass(){Vector3f v(GameObject::CenterOfMass[0], GameObject::CenterOfMass[1], GameObject::CenterOfMass[2]); return v;}
	void GameActiveObject::CalculateCenterOfMass();
	void GameActiveObject::IncrementVelocity(Vector3f& deltaV){velocity += Vec3f(deltaV.x(), deltaV.y(), deltaV.z());}
	void GameActiveObject::IncrementVelocity(Vec3f& deltaV){velocity += deltaV;}
};
#endif _GAMEACTIVEOBJECT_H_