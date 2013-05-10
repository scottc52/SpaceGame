/*
 * An item object.  These are things that can be carried in the inventory.  They are a type of world object.
 *
 */
#ifndef _GAMEITEM_H_
#define _GAMEITEM_H_
#include "GameObject.h"

class GameItem : public GameObject{
private: 
	bool isFound;
	bool isUsed;
	int numUses;
public:
	//to do: Fill in here
};
#endif _GAMEITEM_H_