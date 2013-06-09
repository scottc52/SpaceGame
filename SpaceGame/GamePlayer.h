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
	Weapon* PrimaryWeapon;
	Weapon* SecondaryWeapon;
	GameItem* activeItem;
	Weapon* activeWeapon;
	bool primaryActive;
	//float Max rotation?
	//GameInventory* inventory;
public:
	GamePlayer::GamePlayer(){
		PrimaryWeapon = new SmokeyBulletWeapon(10/*speed*/, 0.8f, 0.6f, 0.4f, 1.0f);
		SecondaryWeapon = new SlugBulletWeapon(25);
		activeItem = NULL;
		activeWeapon = PrimaryWeapon;
		primaryActive = true;
	}
	void GamePlayer::ApplyMovementForce(const Vector3f &force, const double& timeIncrement);
	void GamePlayer::ApplyExternalForce(const Vector3f &force, const Vector3f& position);
	void GamePlayer::SetHealth(const int& h){health = h;}
	int GamePlayer::GetHealth(){return health;}
	Weapon* GamePlayer::GetPrimaryWeapon(){return PrimaryWeapon;}
	void GamePlayer::GetPrimaryWeapon(Weapon* w){PrimaryWeapon = w;}
	Weapon* GamePlayer::GetSecondaryWeapon(){return SecondaryWeapon;}
	void GamePlayer::SetSecondaryWeapon(Weapon* w){SecondaryWeapon = w;}
	GameItem* GamePlayer::GetActiveItem(){return activeItem;}
	void GamePlayer::SetActiveItem(GameItem* i){activeItem = i;}
	Weapon* GamePlayer::GetActiveWeapon(){return activeWeapon;}
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