/*
 * Door object for game.
 * Placed in the world.  Once activated, the player moves from one room to the next, and a new room is loaded.
 */
#ifndef _GAMEDOOR_H_
#define _GAMEDOOR_H_
#include "GameObject.h"
#include "GameItem.h"

class GameDoor : public GameObject{
private:
	char* nextRoom;
	bool isUnlocked;
	GameItem* requiredItem;
public:
	//To do: Fill in here.

};
#endif _GAMEDOOR_H_