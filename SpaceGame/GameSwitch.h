/*
 * Gravity switch object w000t.
 */
#ifndef _GAMESWITCH_H_
#define _GAMESWITCH_H_
#include "GameEventObject.h"
#include "GameItem.h"

class GameSwitch : public GameEventObject{
public:
	void ActivateIfConditionsMet() {
		// Only activate if not activated already...
		if (!isActivated) {
			Vec3f currPos = this->GetPosition();
			Vec3f playerPos = gameState->GetPlayerPosition();
			Vec3f distance = currPos - playerPos;
			if (distance.length() < 5.0) {
				isActivated = true;
				Activate();
			}
		}
	}

	void Activate() {
		// Switch gravity direction via hook into physics engine?
	}

};
#endif _GAMESWITCH_H_