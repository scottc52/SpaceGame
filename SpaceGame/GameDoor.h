/*
 * Door object for game.
 * Placed in the world.  Once activated, the player moves from one room to the next, and a new room is loaded.
 */
#ifndef _GAMEDOOR_H_
#define _GAMEDOOR_H_
#include "GameEventObject.h"
#include "GameItem.h"

class GameDoor : public GameEventObject{
private:
	char* nextRoom;
	bool isUnlocked;
	GameItem* requiredItem;
public:
	void ActivateIfConditionsMet() {
		// Only activate if not activated already...
		if (!isActivated) {
			Vec3f currPos = this->GetPosition();
			Vec3f playerPos = gameState->GetPlayerPosition();
			Vec3f distance = currPos - playerPos;
			if (distance.length() < 10.0) {
				isActivated = true;
				Activate();
			}
		}
	}

	void Activate() {
		// I want this to set a force to make the door move up
		// but I'm not sure how to access forces at this point.
		SetPosition(GetPosition() + Vec3f(0.f, 10.f, 0.f));
	}

};
#endif _GAMEDOOR_H_