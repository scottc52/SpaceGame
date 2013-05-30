#include "GamePlayer.h"

void GamePlayer::FireWeapon(){
	GameItem* weapon = this->GetActiveWeapon();
	//To do: Make weapon fire bullet/Perform action
}

void GamePlayer::UseItem(){
	GameItem* item = this->GetActiveItem();
	//To do: Use the item in the game World.
}

void GamePlayer::ApplyMovementForce(const Vec3f& force, const double& timeIncrement){
	Vec3f IaccelerationDT = Vec3f(force[0]*timeIncrement/this->GetMass(),
							   force[1]*timeIncrement/this->GetMass(),
							   force[2]*timeIncrement/this->GetMass());
	this->IncrementVelocity(IaccelerationDT);
}

void GamePlayer::ApplyExternalForce(const Vec3f& force, const Vec3f& position){

}