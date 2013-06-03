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
	Vector3f com;
	double totalMass;
	string state;
	Vector3f velocity;
	//Rotation information
public:
	GameActiveObject::GameActiveObject(){
		velocity = Vector3f(0,0,0);
		state = "neutral";
	}

	void GameActiveObject::SetState(const char* s){state = s;}
	const char* GameActiveObject::GetState(){return state.c_str();}
	double GameActiveObject::GetMass(){return totalMass;}
	void GameActiveObject::GetMass(const double& m){totalMass = m;}
	Vector3f GameActiveObject::CenterOfMass(){return com;}
	void GameActiveObject::CalculateCenterOfMass();
	void GameActiveObject::IncrementVelocity(Vector3f& deltaV){velocity += deltaV;}
};
#endif _GAMEACTIVEOBJECT_H_