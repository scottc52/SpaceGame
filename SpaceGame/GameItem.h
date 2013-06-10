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
	GameItem::GameItem(){objType = ITEM_TYPE;}
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

#include "GameTime.h"; 
#include "projectile_particles.h"

class Weapon : public GameObject{
public:
	GameTime::GameTimer lastShot;
	double fireInterval; //ms between shots 
	Weapon(){lastShot = GameTime::GetTime(); fireInterval=1.0;}
	virtual ~Weapon(){}
	Projectile *fire(Vector3f &loc, Vector3f &direction){
		if(GameTime::DiffTimeMS(lastShot) < fireInterval)
			return NULL; 
		lastShot = GameTime::GetTime();
		return newProjectile(loc, direction);
	}
	virtual Projectile *newProjectile(Vector3f &loc, Vector3f &direction) = 0; 
};

class SmokeyBulletWeapon : public Weapon{
	private:
		float r, g, b, a; 
		float speed;
	public:
		SmokeyBulletWeapon(float s1, float r1 = 0.5f, float g1 = 0.5f, float b1 = 0.5f, float a1 = 0.5f)
		: Weapon(){speed=s1; r = r1; g = g1; b = b1; a = a1;
		}  
		Projectile* newProjectile(Vector3f &loc, Vector3f &direction){
			Vector3f tmp = direction * speed;
			return new SmokyBullet(loc, tmp, r, g, b, a);
		}
};

class SlugBulletWeapon : public Weapon{
	private:
		float r, g, b, a; 
		float speed;
		float scatter; 
	public:
		SlugBulletWeapon(float s1, float scatter = 1.0f,  float r1 = 0.9f, float g1 = 0.7f, float b1 = 0.4f, float a1 = 0.8f)
		: Weapon(){speed=s1; r = r1; g = g1; b = b1; a = a1;
		}  
		Projectile* newProjectile(Vector3f &loc, Vector3f &direction){
			Vector3f tmp = direction * speed;
			return new Slug(loc, tmp, r, g, b, a);
		}
}; 

#endif _GAMEITEM_H_