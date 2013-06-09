#include "GamePlayer.h"

void GamePlayer::FireWeapon(){
	Weapon* weapon = this->GetActiveWeapon();
	//To do: Make weapon fire bullet/Perform action
}

void GamePlayer::UseItem(){
	GameItem* item = this->GetActiveItem();
	//To do: Use the item in the game World.
}

void GamePlayer::ApplyMovementForce(const Vector3f& force, const double& timeIncrement){
	Vector3f IaccelerationDT = Vector3f(force[0]*timeIncrement/this->GetMass(),
							   force[1]*timeIncrement/this->GetMass(),
							   force[2]*timeIncrement/this->GetMass());
	this->IncrementVelocity(IaccelerationDT);
}

void GamePlayer::ApplyExternalForce(const Vector3f& force, const Vector3f& position){

}