/*
* The player class: includes AI and our main player.
*
*/

#ifndef _GAME_PLAYER_H
#define _GAME_PLAYER_H

#include "GameActiveObject.h"
#include "GameItem.h"

class GamePlayer : GameActiveObject{
private:
	int health;
	GameItem* PrimaryWeapon;
	GameItem* SecondaryWeapon;
	GameItem* activeItem;
	GameItem* activeWeapon;
	bool primaryActive;
	//float Max rotation?
	//GameInventory* inventory;
public:
	GamePlayer::GamePlayer(){
		PrimaryWeapon = NULL;
		SecondaryWeapon = NULL;
		activeItem = NULL;
		activeWeapon = NULL;
		primaryActive = true;
	}
	void GamePlayer::ApplyMovementForce(const Vec3f &force, const double& timeIncrement);
	void GamePlayer::ApplyExternalForce(const Vec3f &force, const Vec3f& position);
	void GamePlayer::SetHealth(const int& h){health = h;}
	int GamePlayer::GetHealth(){return health;}
	GameItem* GamePlayer::GetPrimaryWeapon(){return PrimaryWeapon;}
	void GamePlayer::GetPrimaryWeapon(GameItem* w){PrimaryWeapon = w;}
	GameItem* GamePlayer::GetSecondaryWeapon(){return SecondaryWeapon;}
	void GamePlayer::SetSecondaryWeapon(GameItem* w){SecondaryWeapon = w;}
	GameItem* GamePlayer::GetActiveItem(){return activeItem;}
	void GamePlayer::SetActiveItem(GameItem* i){activeItem = i;}
	GameItem* GamePlayer::GetActiveWeapon(){return activeWeapon;}
	void GamePlayer::SwitchWeapons(){
		if(primaryActive){
			if(SecondaryWeapon != NULL){
				activeWeapon = SecondaryWeapon;
				primaryActive = false;
			}
		}else{
			if(PrimaryWeapon != NULL){
				primaryActive = true;
				activeWeapon = PrimaryWeapon;
			}
		}
	}
	void GamePlayer::FireWeapon();
	void GamePlayer::UseItem();
};

#endif _GAME_PLAYER_H