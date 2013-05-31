#include "GameDoor.h"
#include "GameState.h"

void GameDoor::ActivateIfConditionsMet() {
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

void GameDoor::Activate() {
		// I want this to set a force to make the door move up
		// but I'm not sure how to access forces at this point.
		SetPosition(GetPosition() + Vec3f(0.f, 10.f, 0.f));
}