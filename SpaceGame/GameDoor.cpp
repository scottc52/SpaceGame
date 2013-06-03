#include "GameDoor.h"
#include "GameState.h"

void GameDoor::ActivateIfConditionsMet() {
		// Only activate if not activated already...
		if (!isActivated) {
			Vector3f currPos = this->GetPosition();
			Vector3f playerPos = gameState->GetPlayerPosition();
			Vector3f distance = currPos - playerPos;
			if (distance.norm() < 10.0) {
				isActivated = true;
				Activate();
			}
		}
	}

void GameDoor::Activate() {
		// I want this to set a force to make the door move up
		// but I'm not sure how to access forces at this point.
		Vector3f nPos = GetPosition() + Vector3f(0.f, 10.f, 0.f);
		SetPosition( nPos );
}