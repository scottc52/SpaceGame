#include "GameEventObject.h"
#include "GameState.h"

GameEventObject::GameEventObject(GameState *gameState) {
	isActivated = false;
	this->gameState = gameState;
}

// Default constructor, so that we can use C++'s data 
// structures to store GameEventObjects.
// This should never actually be used as an actual 
// constructor, though, since it'll probably just lead 
// to nasty null pointer errors.
GameEventObject::GameEventObject() {
	isActivated = false;
	this->gameState = NULL;
}