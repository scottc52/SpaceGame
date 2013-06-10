/* 
*
* Basic World Object class.  These objects do not move during the game (Walls, etc.).
*/
#ifndef _GAMEWORLDOBJECT_H_
#define _GAMEWORLDOBJECT_H_
#include "GameObject.h"

class GameWorldObject : public GameObject
{
public:
	GameWorldObject::GameWorldObject(){objType = WORLD_OBJECT_TYPE;}

};
#endif