/*
 * Active (movable/destructible) object for game.
 * Placed in the world.  State machine operates on these things.
 */
#ifndef _GAMEACTIVEOBJECT_H_
#define _GAMEACTIVEOBJECT_H_
#include "GameObject.h"

class GameActiveObject : public GameObject{
private:
	Vec3f CenterOfMass;
	vector<GameObject*> collidedMotionObjects;
public:
	GameActiveObject::GameActiveObject(){}

};
#endif _GAMEACTIVEOBJECT_H_