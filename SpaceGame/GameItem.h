/*
 * An item object.  These are things that can be carried in the inventory.  They are a type of world object.
 *
 */
#ifndef _GAMEITEM_H_
#define _GAMEITEM_H_
#include "GameObject.h"
#include <limits.h>

#define GAMEITEM_ACQUIRED (0)
#define GAMEITEM_NOT_ACQUIRED (1)
//If uses == -1, then don't decrement or increment.
#define GAMEITEM_INFINITE_USES (-1)

class GameItem : public GameObject{
private: 
	int numUses;
	int acquired;
public:
	GameItem::GameItem(){}
	void GameItem::SetAcquired(const int &a){acquired = a;}
	int GameItem::SetAcquired(){return acquired;}
	void GameItem::SetNumUses(const int& u){numUses = u;}
	int GameItem::DecrementUses(){
		if(numUses != GAMEITEM_INFINITE_USES && numUses > 0) numUses --;
	}
	int GameItem::IncrementUses(){
		if(numUses != GAMEITEM_INFINITE_USES && numUses < INT_MAX) numUses ++;
	}
	int GameItem::GetNumUses(){return numUses;}
	//to do: Fill in here
};
#endif _GAMEITEM_H_